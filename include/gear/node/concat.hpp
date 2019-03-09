#ifndef gear_node_concat_hpp_
#define gear_node_concat_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>
#include <gear/vectordeque.hpp>

namespace gear {

  template <typename NumT>
  class concat_node : public flow_node {
    int _nports;
    std::vector<std::shared_ptr<vector_data_info<NumT> > > _infos;
    std::vector<vector_deque<NumT> > _buffers;
    std::vector<bool> _EOS;
    size_t _ndims;

    packet_handler_set _output;

  public:
    concat_node(const std::string& id, const std::string& type)
      : flow_node(id, type) {
    }

    void set_nports(int n) {
      _nports = n;
      _infos.assign(n, std::shared_ptr<vector_data_info<NumT> >());
      _buffers.assign(n, vector_deque<NumT>());
      _EOS.assign(n, false);
    }

    void on_BOS(int port, const BOS_packet& packet) {
      if (port >= _nports) 
        throw std::runtime_error("Attempt to use undeclared port"  + std::to_string(port));

      bool first = true;
      for (int p = 0; p < _nports; ++ p) {
        if (_infos[p]) {
          first = false;
          break;
        }
      }
      if (first) {
        TRACE("First BOS received, clear EOS flags [Port=%d, %lx]", port, this);
        _EOS.assign(_nports, false);
      }

      _infos[port] = std::dynamic_pointer_cast<vector_data_info<NumT> >(packet.datainfo);
      assert(_infos[port].get() != 0);
      _buffers[port] = vector_deque<NumT>(_infos[port]->elems);

      bool done = true;
      for (int p = 0; p < _nports; ++ p) {
        if (! _infos[p]) {
          done = false;
          break;
        } 
      }

      if (done) {
        _ndims = 0;
        for (int p = 0; p < _nports; ++ p) {
          _ndims += _infos[p]->elems;
        }
        BOS_packet npacket(new vector_data_info<NumT>(_ndims));
        _output(npacket);

      }
    }

    void flush() {
      size_t nlen = 0xFFFFFFFF;
      for (int p = 0; p < _nports; ++ p) {
        if (! _infos[p]) {
          nlen = 0; break; // no BOS yet
        }
        nlen = std::min(_buffers[p].size(), nlen);
      }
      if (nlen == 0) return;
      
      numeric<float>::matrix ndata(_ndims, nlen);
      for (int t = 0; t < nlen; ++ t) {
        int doff = 0;
        for (int p = 0; p < _nports; ++ p) {
          ndata.col(t).segment(doff, _infos[p]->elems) = 
            _buffers[p].at(t);
          doff += _infos[p]->elems;
        }
      }
      for (int p = 0; p < _nports; ++ p) {
        _buffers[p].pop_front(nlen);
      }

      vector_data_packet<float> npacket(ndata);
      _output(npacket);
    }

    void on_data(int port, const vector_data_packet<float>& packet) {
      if (port >= _nports) 
        throw std::runtime_error("Attempt to use undeclared port"  + std::to_string(port));
      if (! _infos[port])
        throw std::runtime_error("No BOS for this port");
      _buffers[port].push_back(packet.data); 
      flush();
    }
    
    void on_EOS(int port, const EOS_packet& packet) {
      if (port >= _nports) 
        throw std::runtime_error("Attempt to use undeclared port"  + std::to_string(port));
      _EOS[port] = true;

      flush();

      bool done = true;
      for (int p = 0; p < _nports; ++ p) {
        if (! _EOS[p]) {
          done = false;
          break;
        }
      }
      if (done) {
        // BOS initialize EOS, and EOS initialize infos
        TRACE("Last EOS received, clear BOS infos");
        _infos.assign(_nports, std::shared_ptr<vector_data_info<NumT> >());
        _output(packet);
      }
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() == 0) throw port_not_defined(*this, portname);

      int p = -1;
      try {
        p = std::stoi(portname);
      } catch(...) {
        throw std::runtime_error("Invalid port name");
      }
      if (p < 0 || p >= _nports) 
        throw std::runtime_error("Attempt to use undeclared port" + std::to_string(p));
      
      packet_handler_chain* hdr = new packet_handler_chain;
      hdr->add<vector_data_packet<NumT> >(this,
                                          boost::bind(&concat_node::on_data,
                                                      _1, p, _2))
        ->template add<BOS_packet>(this,
                                   boost::bind(&concat_node::on_BOS, _1, p, _2))
        ->template add<EOS_packet>(this, 
                                   boost::bind(&concat_node::on_EOS, _1, p, _2));
      if (p == 0)
        hdr->add_thru<control_packet>(&this->_output);
      
      return packet_handler_ptr(hdr);
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }
    
    virtual flow_node_ptr clone() const {
      concat_node* ret = new concat_node(get_id(), get_type());
      ret->set_nports(_nports);
      return flow_node_ptr(ret);
    }


  };
}

#endif
