#include <gear/node/factory.hpp>
#include <gear/io/logging.hpp>

namespace gear {
  static node_factory* g_node_factory = 0;

  node_factory& node_factory::get_instance() {
    if (g_node_factory == 0) {
      g_node_factory = new node_factory();
    }
    return *g_node_factory;
  }

  void node_factory::register_factory(const std::string& tname,
                                      const factory_func& factory) {
    TRACE("Factory for %s is regsitered", tname.c_str());
    _factories.insert(std::make_pair(tname, factory));
  }

  node_factory::factory_func
  node_factory::get_factory(const std::string& tname) {
    auto it = _factories.find(tname);
    if (it == _factories.end()) {
      ERROR("Factory for %s is not found", tname.c_str());
      INFO("Registered factories = ");
      for (auto it = _factories.cbegin(), last = _factories.cend(); it != last;
           ++ it) {
        INFO("  %s", it->first.c_str());
      }
      throw std::runtime_error("factory for " + tname + " is not found");
    }
    return it->second;
  }
}

