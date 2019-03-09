#ifndef gear_node_meannorm_hpp_
#define gear_node_meannorm_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>
#include <gear/vectordeque.hpp>

namespace gear {
  class batch_meannorm_node : public flow_node {
    packet_handler_set _output;

    bool _is_first;
    vector_deque<float> _buffer;
  public:
    batch_meannorm_node(const std::string& id, const std::string& type);

    void on_BOS(const BOS_packet& packet);
    void on_EOS(const EOS_packet& packet);
    void on_data(const vector_data_packet<float>& packet);
    
    virtual packet_handler_ptr input_port(const std::string& portname);
    
    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      auto ret = new batch_meannorm_node(get_id(), get_type());
      return flow_node_ptr(ret);
    }

  };
}

#endif
