#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/elemfunc.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(elemwise_func_nodeTest, log) {
    elemwise_func_node node("test", "elemwise_func_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));

    int D = 2, T = 3;
    numeric<float>::matrix source(D, T);
    source << 
      3.14, 1.59, 2.65,
      2.71, 8.28, 1.82;
    numeric<float>::matrix expect(D, T);
    expect << 
      1.144228, 0.46373402,  0.97455964,
      0.99694863,  2.11384297,  0.5988365; // computed with numpy

    node.set_func_type(elemwise_func_node::func_log);

    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
    sink.on_BOS(bospacket);


    vector_data_packet<float> packet(source);
    node.on_data(packet);

    numeric<float>::matrix out = sink.get_output();
    assert_eq_matrix_Linf<float>(expect, out, 1e-5);
  }
}

