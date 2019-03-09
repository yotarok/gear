#ifndef gear_feed_hpp_
#define gear_feed_hpp_

#include <Eigen/Eigen>

namespace gear {

  // This function only supports multiple input streams with same type
  void feed_forward(flow_ptr flow,
                    const std::vector<flow_node_ptr>& inputs) {
    for (auto p: inputs)
      std::dynamic_pointer_cast<source_node>(p)->feed_BOS();
    for (auto p: inputs)  
      std::dynamic_pointer_cast<source_node>(p)->feed_data_all();
    for (auto p: inputs)  
      std::dynamic_pointer_cast<source_node>(p)->feed_EOS();
  }
}

#endif
