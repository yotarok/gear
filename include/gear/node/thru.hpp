#ifndef gear_node_thru_hpp_
#define gear_node_thru_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>

namespace gear {

  class thru_node : public flow_node {
    packet_handler_set _output;

  public:
    thru_node(const std::string& id, const std::string& type)
      : flow_node(id, type) { }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      
      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->add_thru<packet>(&this->_output)
                           );
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }
    
    virtual flow_node_ptr clone() const {
      auto ret = new thru_node(get_id(), get_type());
      return flow_node_ptr(ret);
    }

  };
}

#endif
