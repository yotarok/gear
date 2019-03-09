#include <gear/node/delta.hpp>

#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_delta_node(const YAML::Node& src) {
    delta_node* ret =
      new delta_node(src["name"].as<std::string>(),
                     src["type"].as<std::string>());
    if (src["halfwidth"]) {
      ret->set_halfwidth(src["halfwidth"].as<int>());
    }
    return flow_node_ptr(ret);
  }

  static node_factory_registry
  delta_node_registry("delta", create_delta_node);


  delta_node::delta_node(const std::string& id, const std::string& type)
    : window_node(id, type) {
  }

  void delta_node::set_halfwidth(int n) { 
    _halfwidth = n; 
    set_shift(1);
    set_size(n * 2 + 1);
    set_left_padding(window_node::padding_repeat, n);
    set_right_padding(window_node::padding_repeat, n);
  }

  size_t delta_node::output_dims() {
    return _input_dims;
  } 

  void delta_node::initialize(const BOS_packet& packet) {
  }

  numeric<float>::matrix delta_node::setup_output_data() {
    int nlen = (_buffer.size() - _size) / _shift + 1;
    
    numeric<float>::matrix ndata = numeric<float>::matrix::Zero(_input_dims, nlen);
    float denom = 0.0;
    for (int tau = 1; tau <= _halfwidth; ++ tau) {
      denom += tau * tau;
    }
    denom *= 2.0;
    
    for (int i = 0; i < nlen; ++ i) {
      for (int tau = 1; tau <= _halfwidth; ++ tau) {
        ndata.col(i) += static_cast<float>(tau) / denom *
          (_buffer.at(_shift * (_halfwidth + i + tau)) - 
           _buffer.at(_shift * (_halfwidth + i - tau)));
      }
    }
    return ndata;
  }

  flow_node_ptr delta_node::clone() const {
    delta_node* ret = new delta_node(get_id(), get_type());
    ret->set_halfwidth(_halfwidth);
    return flow_node_ptr(ret);
  }

}
