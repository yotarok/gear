#include <gear/node/elemfunc.hpp>

#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_elemwise_func_node(const YAML::Node& src) {
    elemwise_func_node* ret =
      new elemwise_func_node(src["name"].as<std::string>(),
                             src["type"].as<std::string>());
    if (src["function"]) {
      ret->set_func_type(src["function"].as<std::string>());
    }
    return flow_node_ptr(ret);
  }

  static node_factory_registry
  elemwise_func_node_registry("elemwise_func", create_elemwise_func_node);

  elemwise_func_node::elemwise_func_node(const std::string& id, const std::string& type)
    : flow_node(id, type), _func(func_id) {
  }
    
  void elemwise_func_node::set_func_type(func_type t) {
    _func = t;
  }

  void elemwise_func_node::set_func_type(const std::string& nam) {
    if (nam == "id") {
      set_func_type(func_id);
    } else if (nam == "log") {
      set_func_type(func_log);
    } else {
      throw std::runtime_error("unknown functype");
    }
  }

  void elemwise_func_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;
    int T = packet.data.cols();
    
    numeric<float>::matrix ndata;
    switch (_func) {
    case func_id:
      ndata = packet.data;
      break;
    case func_log:
      ndata = packet.data.array().log().matrix();
      break;
    default:
      throw std::runtime_error("unknown functype");
    }
    
    _output(vector_data_packet<float>(ndata));
  }

  packet_handler_ptr elemwise_func_node::input_port(const std::string& portname) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
      
    return 
      packet_handler_ptr((new packet_handler_chain)
                         ->add<vector_data_packet<float> >(this, &elemwise_func_node::on_data)
                         ->add_thru<control_packet>(&this->_output)
                         );
  }
  
    
  void elemwise_func_node::connect(const std::string& portname,
                                   packet_handler_ptr dest) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
    _output.insert(dest);
  }
  
  flow_node_ptr elemwise_func_node::clone() const {
    elemwise_func_node* ret = new elemwise_func_node(get_id(), get_type());
    ret->set_func_type(_func);
    return flow_node_ptr(ret);
  }


}
