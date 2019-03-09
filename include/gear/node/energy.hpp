#ifndef gear_node_energy_hpp_
#define gear_node_energy_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <gear/io/logging.hpp>

namespace gear {
  class energy_node : public flow_node {
    packet_handler_set _output;
  public:
    energy_node(const std::string& id, const std::string& type)
      : flow_node(id, type) {
    }

    void on_BOS(const BOS_packet& packet) {
      BOS_packet npacket(new vector_data_info<float>(1));
      _output(npacket);
    }

    void on_data(const vector_data_packet<float>& packet) {
      if (packet.data.cols() == 0) return;

      int T = packet.data.cols();      
      vector_data_packet<float> npacket(numeric<float>::matrix::Zero(1, T));

      for (int t = 0; t < T; ++ t) {
        npacket.data(0, t) = packet.data.col(t).squaredNorm();
      }
      _output(npacket);
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      
      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->add<vector_data_packet<float> >(this, &energy_node::on_data)
                           ->add<BOS_packet>(this, &energy_node::on_BOS)
                           ->add_thru<control_packet>(&this->_output)
                         );
    }


    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      energy_node* ret = new energy_node(get_id(), get_type());
      return flow_node_ptr(ret);
    }

  };
}

#endif
