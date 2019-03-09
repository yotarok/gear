#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/energy.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;
  TEST(energy_nodeTest, WithoutPadding) {
    energy_node node("e", "energy_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));
    
    int D = 3, T = 4;
    numeric<float>::matrix source(D, T);
    source << 
      2, 5, -2, 4,
      3, 2, 1,  0,
      -3, 4, 1, 1;

    numeric<float>::matrix expect(1, T);
    expect <<
      22, 45, 6, 17;


    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
    node.on_BOS(bospacket);

    vector_data_packet<float> packet(source);
    node.on_data(packet); 

    numeric<float>::matrix out = sink.get_output();

    assert_eq_matrix_Linf<float>(expect, out, 1e-8);
  }
}
