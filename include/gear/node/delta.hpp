#ifndef gear_node_delta_hpp_
#define gear_node_delta_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <gear/node/window.hpp>

namespace gear {
  class delta_node : public window_node<float> {
    int _halfwidth;
  public:
    delta_node(const std::string& id, const std::string& type);

    void set_halfwidth(int n);
    
    virtual size_t output_dims();

    virtual void initialize(const BOS_packet& packet);

  protected:
    virtual numeric<float>::matrix setup_output_data();
    
    virtual flow_node_ptr clone() const;
    
  };

}

#endif
