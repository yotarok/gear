#ifndef gear_node_elemfunc_hpp_
#define gear_node_elemfunc_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>

namespace gear {
  class elemwise_func_node : public flow_node {
  public:
    enum func_type {
      func_id,
      func_log,
    };
  private:
    func_type _func;

    packet_handler_set _output;

  public:
    elemwise_func_node(const std::string& id, const std::string& type);
    
    void set_func_type(func_type t);
    void set_func_type(const std::string& nam);

    void on_data(const vector_data_packet<float>& packet);
    
    virtual packet_handler_ptr input_port(const std::string& portname);
    
    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest);

    virtual flow_node_ptr clone() const;

  };
}

#endif
