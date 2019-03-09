#ifndef gear_node_factory_hpp_
#define gear_node_factory_hpp_

#include <gear/node/node.hpp>
#include <map>
#include <functional>


namespace gear {
  class node_factory {
  public:
    typedef std::function<flow_node_ptr (const YAML::Node&)> factory_func;
  private:
    std::map<std::string, factory_func> _factories;
    node_factory() { }
  public:
    static node_factory& get_instance();
    void register_factory(const std::string& tname,
                          const factory_func& factory);
    factory_func get_factory(const std::string& tname);

  };

  struct node_factory_registry {
    bool registered;
    node_factory_registry(const std::string& tname,
                          node_factory::factory_func func) {
      node_factory::get_instance().register_factory(tname, func);
      registered = true;
    }
  };
}

#endif
