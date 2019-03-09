#include <gear/node/window.hpp>

#include <gear/node/factory.hpp>
#include <gear/io/matrix.hpp>

namespace gear {
  template <typename NumT>
  struct create_window_node {
    flow_node_ptr operator()(const YAML::Node& src) {
      window_node<NumT>* ret =
        new window_node<NumT>(src["name"].as<std::string>(),
                              src["type"].as<std::string>());


      if (src["size"]) {
        ret->set_size(src["size"].as<int>());
      }
      if (src["shift"]) {
        ret->set_shift(src["shift"].as<int>());
      }
      if (src["leftpad"]) {
        std::cout << " set left pad " << src["leftpad"]["type"].as<std::string>() << " : " << src["leftpad"]["size"].as<int>() << std::endl;
        ret->set_left_padding(src["leftpad"]["type"].as<std::string>(), 
                              src["leftpad"]["size"].as<int>());
      }
      if (src["rightpad"]) {
        std::cout << " set right pad " << src["rightpad"]["type"].as<std::string>() << " : " << src["rightpad"]["size"].as<int>() << std::endl;
        ret->set_right_padding(src["rightpad"]["type"].as<std::string>(), 
                               src["rightpad"]["size"].as<int>());
      }
      return flow_node_ptr(ret);
    }
  };
    
  static node_factory_registry
  window_node_registry("window",
                       create_window_node<float>());

  static node_factory_registry
  intwindow_node_registry("intwindow",
                           create_window_node<int>());
  
}
