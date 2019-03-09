#include <gear/node/energy.hpp>
#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_energy_node(const YAML::Node& src) {
    energy_node* ret =
      new energy_node(src["name"].as<std::string>(),
                      src["type"].as<std::string>());
    return flow_node_ptr(ret);
  }
  static node_factory_registry
  energy_node_registry("energy", create_energy_node);
}
