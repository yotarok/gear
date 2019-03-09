#include <gear/node/htkcompat.hpp>

#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_HTK_preemph_node(const YAML::Node& src) {
    HTK_preemph_node* ret = new HTK_preemph_node(src["name"].as<std::string>(),
                                                 src["type"].as<std::string>());
    if (src["coef"]) {
      ret->set_coef(src["coef"].as<float>());
    }
    return flow_node_ptr(ret);
  }
  static node_factory_registry HTK_preemph_node_registry("HTK_preemph", create_HTK_preemph_node);


  flow_node_ptr create_HTK_cepstral_scaler_node(const YAML::Node& src) {
    HTK_cepstral_scaler_node* ret =
      new HTK_cepstral_scaler_node(src["name"].as<std::string>(),
                                   src["type"].as<std::string>());
    if (src["offset"]) {
      ret->set_offset(src["offset"].as<int>());
    }
    if (src["const"]) {
      ret->set_const(src["const"].as<int>());
    }
    return flow_node_ptr(ret);
  }
  static node_factory_registry
  HTK_cepstral_scaler_registry("HTK_cepstral_scaler",
                               create_HTK_cepstral_scaler_node);


  
  HTK_preemph_node::HTK_preemph_node(const std::string& id,
                                     const std::string& type)
    : flow_node(id, type), _coef(0.97) {
  }

  void HTK_preemph_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;

    numeric<float>::matrix ndata = packet.data;
    for (int t = 0; t < packet.data.cols(); ++ t) {
      for (int tau = packet.data.rows() - 1; tau >= 1; -- tau) {
        ndata(tau, t) -= ndata(tau - 1, t) * _coef;
      }
      ndata(0, t) *= 1.0 - _coef;
    }
    _output(vector_data_packet<float>(ndata));

  }

  packet_handler_ptr HTK_preemph_node::input_port(const std::string& portname) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
    
    return 
      packet_handler_ptr((new packet_handler_chain)
                         ->add<vector_data_packet<float> >(this, &HTK_preemph_node::on_data)
                         ->add_thru<control_packet>(&this->_output)
                         );
  }

  void HTK_preemph_node::connect(const std::string& portname,
                                 packet_handler_ptr dest) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
    _output.insert(dest);
  }
  
  flow_node_ptr HTK_preemph_node::clone() const {
    HTK_preemph_node* ret = new HTK_preemph_node(get_id(), get_type());
    ret->set_coef(_coef);
    return flow_node_ptr(ret);
  }


}
