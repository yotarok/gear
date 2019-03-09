#ifndef gear_flow_flow_hpp_
#define gear_flow_flow_hpp_

#include <gear/node/node.hpp>
#include <boost/tuple/tuple.hpp>
#include <iostream>

namespace gear {
  
  struct link_descr {
    flow_node_ptr from;
    std::string from_port;
    flow_node_ptr to;
    std::string to_port;
  };

  class flow;
  typedef std::shared_ptr<flow> flow_ptr;
  
  class flow {
    std::map<std::string, flow_node_ptr> _nodes;
    std::vector<boost::tuple<std::string, std::string,
                             std::string, std::string> > _linkinfo;
    // ^ used only for cloning
  public:

    flow_ptr clone() const {
      flow* ret = new flow;
      for (auto it = _nodes.cbegin(), last = _nodes.cend();
           it != last; ++ it) {
        ret->add(it->second->clone());
      }
      for (auto it = _linkinfo.cbegin(), last = _linkinfo.cend();
           it != last; ++ it) {
        std::cout << boost::get<0>(*it) << ":" << boost::get<1>(*it) << " => "
                  << boost::get<2>(*it) << ":" << boost::get<3>(*it)
                  << std::endl;
        ret->connect(boost::get<0>(*it), boost::get<1>(*it),
                     boost::get<2>(*it), boost::get<3>(*it));
      }
      return flow_ptr(ret);
    }
    
    // this transfers ownership
    flow_node_ptr add(flow_node* node) {
      flow_node_ptr p(node);
      add(p);
      return p;
    }

    flow_node_ptr add(flow_node_ptr p) {
      _nodes.insert(std::make_pair(p->get_id(), p));
      return p;
    }

    flow_node_ptr node(const std::string& id) {
      auto it = _nodes.find(id);
      if (it == _nodes.end()) {
        throw std::runtime_error(id + " is not found");
      }
      return it->second;
    }

    template <typename RetT>
    std::shared_ptr<RetT> node_as(const std::string& id) {
      std::shared_ptr<RetT> ret = this->node(id);
      if (! ret) {
        throw std::runtime_error("type error");
      }
      return ret;
    }
    
    void connect(const std::string& fromnode, const std::string& fromport,
                 const std::string& tonode, const std::string& toport) {
      _linkinfo.push_back(boost::make_tuple(fromnode, fromport, tonode, toport));
      this->node(fromnode)->connect(fromport, 
                                    this->node(tonode)->input_port(toport));
    }
  };


  flow_ptr parse_flow(const std::string& path);
}

#endif
