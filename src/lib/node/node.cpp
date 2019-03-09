#include <gear/node/node.hpp>
#include <gear/node/factory.hpp>

namespace gear {

  flow_node_ptr create_node(const YAML::Node& src) {
    std::string tname = src["type"].as<std::string>();
    node_factory::factory_func f =
      node_factory::get_instance().get_factory(tname);
    return f(src);
  }

}
