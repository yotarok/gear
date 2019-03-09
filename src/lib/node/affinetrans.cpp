#include <gear/node/affinetrans.hpp>

#include <gear/node/factory.hpp>
#include <gear/io/matrix.hpp>

#include <iostream>

namespace gear {
 
  flow_node_ptr create_affine_transform_node(const YAML::Node& src) {
    affine_transform_node* ret =
      new affine_transform_node(src["name"].as<std::string>(),
                                src["type"].as<std::string>());

    if (src["weight"]) {
      numeric<float>::matrix wmat;
      read_yaml_matrix(&wmat, src["weight"]);
      ret->set_weight(wmat);
    }
    
    if (src["bias"]) {
      numeric<float>::matrix bias;
      read_yaml_matrix(&bias, src["bias"]);
      if (bias.cols() != 1) {
        throw std::runtime_error("Bias must be a vector");
      }
      ret->set_bias(bias.col(0));
    }
    
    if (src["scale"]) {
      numeric<float>::matrix scale;
      read_yaml_matrix(&scale, src["scale"]);
      if (scale.cols() != 1) {
        throw std::runtime_error("Scale must be a vector");        
      }
      ret->set_scale(scale.col(0));
    }
    
    return flow_node_ptr(ret);
  }
  
  static node_factory_registry
  affine_transform_node_registry("affine_transform",
                                 create_affine_transform_node);

  flow_node_ptr create_DCT_node(const YAML::Node& src) {
    DCT_node* ret =
      new DCT_node(src["name"].as<std::string>(),
                   src["type"].as<std::string>());

    if (src["ncoefs"]) {
      ret->set_ncoefs(src["ncoefs"].as<int>());
    }
    if (src["offset"]) {
      ret->set_offset(src["offset"].as<int>());
    }
    
    return flow_node_ptr(ret);
  }
  
  static node_factory_registry DCT_node_registry("DCT",
                                                 create_DCT_node);


  

  flow_node_ptr affine_transform_node::clone() const {
    affine_transform_node* ret =
      new affine_transform_node(get_id(), get_type());
    if (_has_weight) ret->set_weight(_weight);
    if (_has_scale) ret->set_scale(_scale);
    if (_has_bias) ret->set_bias(_bias);
    return flow_node_ptr(ret);
  }
  
  void affine_transform_node::set_weight(const numeric<float>::matrix& w) { 
    _weight = w; 
    _has_weight = true;
    _outdims = _weight.rows();
  }

  void affine_transform_node::set_scale(const numeric<float>::vector& s) { 
    _scale = s; 
    _has_scale = true;
    _outdims = _scale.rows();
  }

  void affine_transform_node::set_bias(const numeric<float>::vector& b) { 
    _bias = b; 
    _has_bias = true;
    _outdims = _bias.rows();
  }

  void affine_transform_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;
    int T = packet.data.cols();
    
    vector_data_packet<float> npacket(numeric<float>::matrix::Zero(_outdims, T));
    if (_has_weight)
      npacket.data = _weight * packet.data;
    else  
      npacket.data = packet.data;
    
    if (_has_bias)
      for (int c = 0; c < npacket.data.cols(); ++ c) 
        npacket.data.col(c) = npacket.data.col(c) + _bias;
    
    if (_has_scale)
      for (int c = 0; c < npacket.data.cols(); ++ c) 
        npacket.data.col(c) = (npacket.data.col(c).array() * _scale.array()).matrix();     
    
    _output(npacket);
  }

  packet_handler_ptr
  affine_transform_node::input_port(const std::string& portname) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
    
    return 
      packet_handler_ptr((new packet_handler_chain)
                         ->add<vector_data_packet<float> >(this, &affine_transform_node::on_data)
                         ->add_thru<control_packet>(&this->_output)
                         );
  }

  void affine_transform_node::connect(const std::string& portname,
                                      packet_handler_ptr dest) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
    _output.insert(dest);
  }
}
