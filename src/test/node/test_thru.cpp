#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/thru.hpp>
#include <gear/node/window.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(ThruTest, ViaWindow) {
    // Since thru_node doesn't have any way to send packet, 
    // test is performed via window_node

    window_node<float> win("test", "window_node");
    thru_node node("thru", "thru_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    win.connect("", node.input_port(""));
    node.connect("", sink.input_port(""));

    int D = 3, T = 4, shift = 1, size = 3;
    numeric<float>::matrix source(D, T);
    source << 
      2, 5, -2, 4,
      3, 2, 1,  0,
      -3, 4, 1, 1;

    numeric<float>::matrix expect(D * size, 2);
    expect <<
      2, 5,
      3, 2,
      -3, 4,
      5, -2,
      2, 1,
      4, 1,
      -2, 4,
      1, 0, 
      1, 1;

    win.set_size(size);
    win.set_shift(shift);

    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
    win.on_BOS(bospacket);

    vector_data_packet<float> packet(source);
    win.on_data(packet); 

    EOS_packet eospacket;
    win.on_EOS(eospacket);

    numeric<float>::matrix out = sink.get_output();

    assert_eq_matrix_Linf<float>(expect, out, 1e-8);
  }

}
