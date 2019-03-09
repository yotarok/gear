#ifndef gear_node_node_hpp_
#define gear_node_node_hpp_

#include <map>
#include <set>
#include <vector>
#include <gear/flow/packet.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <gear/io/logging.hpp>
//#include <functional>
#include <gear/vectordeque.hpp>
#include <yaml-cpp/yaml.h>
#include <memory>

namespace gear {
  class packet_handler {
  public:
    virtual bool feed(const packet& packet) = 0;
  };


  typedef std::shared_ptr<packet_handler> packet_handler_ptr;

  class packet_handler_set : public std::set<packet_handler_ptr> {
  public:
    void operator() (const packet& p) {
      for (auto ptr : *this) {
        ptr->feed(p);
      }
    }
  };

  class flow_node;
  typedef std::shared_ptr<flow_node> flow_node_ptr;

  class flow_node {
  private:
    std::string _id;
    std::string _type;

  protected:
    flow_node(const std::string& id, const std::string& type)
      : _id(id), _type(type) { }

    std::map<std::string, packet_handler_set> _port_map;
  public:
    const std::string& get_id() const { return _id; }
    const std::string& get_type() const { return _type; }
    
    virtual packet_handler_ptr input_port(const std::string& portname)=0;

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest)=0;

    virtual flow_node_ptr clone() const =0;

  };


  template <class PacketT>
  struct typed_packet_handler : public packet_handler {
    boost::function<void (const PacketT&)> impl;

    typed_packet_handler(boost::function<void (const PacketT&)> f) : impl(f) { }
    
    virtual bool feed(const packet& packet) {
      const PacketT* p = dynamic_cast<const PacketT*>(&packet);
      if (p == 0) {
        return false;
      } else {
        impl(*p);
        return true;
      }
    }
  };

  template <class PacketT>
  struct typed_thru_handler : public packet_handler {
    packet_handler_set* dest;

    typed_thru_handler(packet_handler_set* d) : dest(d) { }

    virtual bool feed(const packet& packet) {
      const PacketT* p = dynamic_cast<const PacketT*>(&packet);
      if (p == 0) {
        return false;
      } else {
        (*dest)(*p);
        return true;
      }
    }
  };

  class packet_handler_chain : public packet_handler {
    std::vector<packet_handler_ptr> _handlers;
    
  public:
    packet_handler_chain() { }

   

    virtual bool feed(const packet& packet) {
      for (auto handler : _handlers) {
        if (handler->feed(packet)) return true;
      }
      return false;
    }

    template <typename PacketT, typename FuncT, typename NodeT> 
    packet_handler_chain* override(NodeT* node, FuncT func) {
      _handlers.insert(_handlers.begin(),
                       packet_handler_ptr(new typed_packet_handler<PacketT>(boost::bind(func, dynamic_cast<NodeT*>(node), _1))));
      return this;
    }

    template <typename PacketT, typename FuncT, typename NodeT> 
    packet_handler_chain* add(NodeT* node, FuncT func) {
      _handlers.push_back(packet_handler_ptr(new typed_packet_handler<PacketT>(boost::bind(func, dynamic_cast<NodeT*>(node), _1))));
      return this;
    }

    template <typename PacketT>
    packet_handler_chain* add_thru(packet_handler_set* dest) {
      _handlers.push_back(packet_handler_ptr(new typed_thru_handler<PacketT>(dest)));
      return this;
    }
  };

  struct port_not_defined : public std::runtime_error {
    port_not_defined(const flow_node& node, const std::string& portname) :
      std::runtime_error("Port \"" + portname + "\" is not defined in " + node.get_id() + " [type = " + node.get_type() + "]") {
    }
  };

  class source_node {
  public:
    virtual void feed(packet)=0;
    virtual void feed_BOS()=0;
    virtual void feed_data_all()=0;
    virtual void feed_EOS()=0;
  };

  template <typename NumT>
  class vector_source_node : public flow_node, public source_node {
    vector_deque<NumT> _buffer;

    int _dims;
    int _samprate;
    std::string _filepath;


    packet_handler_set _output;

  public:

    typedef std::shared_ptr<vector_source_node<NumT> > ptr;

    vector_source_node(const std::string& id, const std::string & type) 
      : flow_node(id, type) {
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      throw port_not_defined(*this, portname);
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }
      
    void set_meta_data(int dims, int samprate, const std::string& filepath) {
      _dims = dims;
      _samprate = samprate;
      _filepath = filepath;
    }

    void append_data(const typename numeric<NumT>::matrix& data) {
      if (_buffer.ndims() < 0) {
        _buffer = vector_deque<NumT>(data.rows(), data.cols());
      }
      _buffer.push_back(data);
    }

    void feed(packet) {
    }
    
    virtual void feed_BOS() {
      BOS_packet bospacket;
      bospacket.datainfo = std::shared_ptr<vector_data_info<NumT> >(new vector_data_info<NumT>(_dims));
      _output(bospacket);
    }
    
    virtual void feed_data_all() {
      vector_data_packet<NumT> packet(_buffer.all());
      _buffer.pop_front(_buffer.size());
      _output(packet);
    }
    
    virtual void feed_EOS() {
      EOS_packet eospacket;
      _output(eospacket);      
    }

    virtual flow_node_ptr clone() const {
      auto ret = new vector_source_node<NumT>(get_id(), get_type());
      return flow_node_ptr(ret);
    }

  };


  template <typename NumT>
  class vector_sink_node : public flow_node {
    vector_deque<NumT> _buffer;
    std::shared_ptr<vector_data_info<NumT> > _info;

  public:
    vector_sink_node(const std::string& id, const std::string& type)
      : flow_node(id, type) { 
    }

    void on_BOS(const BOS_packet& packet) {
      _info = std::dynamic_pointer_cast<vector_data_info<NumT> >(packet.datainfo);
      if (! _info) {
        throw std::runtime_error("Input is not vector sequence");
      }
      _buffer = vector_deque<NumT>(_info->elems);
    }

    void on_data(const vector_data_packet<NumT>& packet) {
      if (_buffer.ndims() < 0) {
        WARN("BOS packet is not delivered");
        _buffer = vector_deque<NumT>(packet.data.rows());
      }
      _buffer.push_back(packet.data);
    }

    typename numeric<NumT>::matrix get_output() {
      return _buffer.all();
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      return
        packet_handler_ptr((new packet_handler_chain)
                           ->template add<BOS_packet>(this, &vector_sink_node<NumT>::on_BOS)
                           ->template add<vector_data_packet<NumT> >(this, &vector_sink_node<NumT>::on_data));
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      throw port_not_defined(*this, portname);
    }

    virtual flow_node_ptr clone() const {
      auto ret = new vector_sink_node<NumT>(get_id(), get_type());
      return flow_node_ptr(ret);
    }

  };

  flow_node_ptr create_node(const YAML::Node& src);
  
}

#endif

