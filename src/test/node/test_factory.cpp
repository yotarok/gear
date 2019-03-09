#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/factory.hpp>
#include <gear/test/utils.hpp>

#include <gear/node/fft.hpp>
#include <gear/node/affinetrans.hpp>
#include <gear/node/window.hpp>

namespace {
  using namespace gear;

  TEST(factory_test, InstatiateFFT) {
    const char* yamlsrc =
      "name: test\n"
      "type: FFT\n"
      "window: square\n"
      "nbins: 512\n"
      "power: 2\n";

    YAML::Node yaml = YAML::Load(yamlsrc);
    flow_node_ptr p = node_factory::get_instance().get_factory("FFT")(yaml);

    ASSERT_EQ("test", p->get_id());
    ASSERT_EQ("FFT", p->get_type());
  }

  TEST(factory_test, InstantiateAffine) {
    const char* yamlsrc =
      "name: test\n"
      "type: affine_transform\n"
      "bias:\n"
      "  data: [0.1, 0.2, 0.3]\n"
      "scale:\n"
      "  data: [1.0, 2.0, 0.5]\n"
      "weight:\n"
      "  data:\n"
      "   - [1.0, 0.0, 0.0]\n"
      "   - [1.0, 1.0, 0.0]\n"
      "   - [1.0, 1.0, 1.0]\n";
    
    YAML::Node yaml = YAML::Load(yamlsrc);
    flow_node_ptr p =
      node_factory::get_instance().get_factory("affine_transform")(yaml);
    auto pp = std::dynamic_pointer_cast<affine_transform_node>(p);

    ASSERT_NEAR(1.0, pp->weight()(2, 0), 0.00001);
    ASSERT_NEAR(0.2, pp->bias()(1), 0.00001);
    ASSERT_NEAR(1.0, pp->scale()(0), 0.00001);
    
  }

  TEST(factory_test, InstantiateWindow) {
    const char* yamlsrc =
      "name: test\n"
      "type: window\n"
      "shift: 5\n"
      "size: 11\n"
      "leftpad: {type: repeat, size: 5}\n"
      "rightpad: {type: repeat, size: 5}\n";
    
    YAML::Node yaml = YAML::Load(yamlsrc);
    flow_node_ptr p =
      node_factory::get_instance().get_factory("window")(yaml);

    auto pp = std::dynamic_pointer_cast<window_node<float> >(p);
    
  }

}
