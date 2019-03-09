#include <gear/node/concat.hpp>

#include <gear/node/factory.hpp>
#include <gear/io/matrix.hpp>

namespace gear {
  template <typename NumT>
  struct create_concat_node {
    flow_node_ptr operator()(const YAML::Node& src) {
      concat_node<NumT>* ret =
        new concat_node<NumT>(src["name"].as<std::string>(),
                              src["type"].as<std::string>());
      
      if (src["nports"]) {
        ret->set_nports(src["nports"].as<int>());
      }
      return flow_node_ptr(ret);
    }
  };

  static node_factory_registry
  concat_node_registry("concat",
                       create_concat_node<float>());
  

  /*
    static node_factory_registry
    intconcat_node_registry("intconcat",
    create_concat_node<int>());
  */

}
