#ifndef gear_node_fir_hpp_
#define gear_node_fir_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>

namespace gear {
  // In principle, FIR is Window+AffineTransform. 
  // This node is for simplicity and computational efficiency
  class FIR_node : public flow_node {
    numeric<float>::matrix _leftover;
    numeric<float>::vector _coefs;

    packet_handler_set _output;
  public:
    FIR_node(const std::string& id, const std::string& type)
      : flow_node(id, type) {
    }

    void set_coefs(const numeric<float>::vector& c) {
      _coefs = c;
    }

    void on_BOS(const BOS_packet& packet) {
      int D = packet.get_info<vector_data_info<float> >().elems;
      _leftover = numeric<float>::matrix::Zero(D, _coefs.size() - 1);
      _output(packet);
    }

    void on_data(const vector_data_packet<float>& packet) {
      if (packet.data.cols() == 0) return;
      
      int T = packet.data.cols();
      int D = packet.data.rows();

      numeric<float>::matrix ret = numeric<float>::matrix::Zero(D, T);
      for (int t = 0; t < T; ++ t) {
        for (int tau = 0; tau < _coefs.size(); ++ tau) {
          if (t - tau >= 0) {
            ret.col(t) += _coefs(tau) * packet.data.col(t - tau);
          } else {
            ret.col(t) += _coefs(tau) * _leftover.col(_coefs.size() + (t - tau) - 1);
          }
        }
      }

      if (T < _leftover.cols()) {
        _leftover.block(0, 0, D, _leftover.cols() - T) = 
          _leftover.block(0, T, D, _leftover.cols() - T);
        _leftover.block(0, _leftover.cols() - T, D, T) = packet.data;
      } else {
        _leftover = packet.data.block(0, T - _leftover.cols(), D, _leftover.cols());
      }

      vector_data_packet<float> npacket(ret);
      _output(npacket);
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      
      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->add<vector_data_packet<float> >(this, &FIR_node::on_data)
                           ->add<BOS_packet>(this, &FIR_node::on_BOS)
                           ->add_thru<control_packet>(&this->_output)
                           );
    }


    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      FIR_node* ret = new FIR_node(get_id(), get_type());
      ret->set_coefs(_coefs);
      return flow_node_ptr(ret);
    }

  };
}

#endif
