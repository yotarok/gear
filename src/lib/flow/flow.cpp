#include <gear/flow/flow.hpp>
#include <gear/node/thru.hpp>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <fstream>

namespace gear {
  flow_ptr parse_flow(const std::string& path) {
    std::ifstream ifs(path);
    YAML::Node root;
    try { 
      root = YAML::Load(ifs);
    } catch(...) {
      ERROR("Caught exception while parsing YAML");
      throw;
    }
    flow_ptr ret(new flow);

    auto input = new thru_node("_input", "thru_node");
    auto output = new thru_node("_output", "thru_node");
    ret->add(input);
    ret->add(output);
    
    try { 
      YAML::Node yamlnodelist = root["node"];
      for (int i = 0; i < yamlnodelist.size(); ++ i) {
        flow_node_ptr flownode = create_node(yamlnodelist[i]);
        ret->add(flownode);
      }
    } catch(...) {
      ERROR("Caught exception while creating nodes");
      throw;
    }

    try { 
      YAML::Node connlist = root["connection"];
      for (int i = 0; i < connlist.size(); ++ i) {
        YAML::Node conn = connlist[i];
        std::string fromname = conn["from"].as<std::string>();
        std::string fromport = (conn["from_port"])
          ? conn["from_port"].as<std::string>() : "";
        std::string toname = conn["to"].as<std::string>();
        std::string toport = (conn["to_port"])
          ? conn["to_port"].as<std::string>() : "";
        
        ret->connect(fromname, fromport, toname, toport);
      }
    } catch(...) {
      ERROR("Caught exception while making connections");
      throw;
    }
    return ret;
  }
}
