#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/meannorm.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(batch_meannorm_test, Test) {
    batch_meannorm_node node("test", "batch_meannorm_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");
    node.connect("", sink.input_port(""));

    // zero-mean test
    int D = 3, T = 4;
    numeric<float>::matrix source(D, T);
    source << 
      2, 0, -2,  0,
      3, 2, -4, -1,
      -3, 4, 1, -2;

    numeric<float>::matrix expect = source;

    for (int n = 0; n < 2; ++ n) {
      BOS_packet bospacket;
      bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
      node.on_BOS(bospacket);
      
      vector_data_packet<float> packet(source);
      node.on_data(packet); 
      
      EOS_packet eospacket;
      node.on_EOS(eospacket);

      numeric<float>::matrix out = sink.get_output();
      
      assert_eq_matrix_Linf<float>(expect, out, 1e-8);
    }

  }
}
