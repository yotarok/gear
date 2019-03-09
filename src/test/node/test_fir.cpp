#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/fir.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(FIR_nodeTest, Basic) {
    FIR_node node("test", "FIR_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));

    numeric<float>::matrix source(1, 5);
    source << 
      0.79, 0.29, 0.31, 0.11,  0.81;
    numeric<float>::matrix expect(1, 5);
    expect <<
      0.79, -0.4763, 0.0287, -0.1907, 0.7033;

    numeric<float>::vector coeff(2);
    coeff << 1.0, -0.97;

    node.set_coefs(coeff);
    
    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(1));
    node.on_BOS(bospacket);

    vector_data_packet<float> packet(source);
    node.on_data(packet); 

    //EOS_packet eospacket;
    //node.on_EOS(eospacket);

    numeric<float>::matrix out = sink.get_output();

    assert_eq_matrix_Linf<float>(expect, out, 1e-5);
  }

}
