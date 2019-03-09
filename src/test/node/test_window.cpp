#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/window.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(window_node_test, WithoutPadding) {
    window_node<float> node("test", "window_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

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

    node.set_size(size);
    node.set_shift(shift);

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


  TEST(window_node_test, ZeroPadding) {
    window_node<float> node("test", "window_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));

    int D = 3, T = 4, shift = 1, size = 3;
    numeric<float>::matrix source(D, T);
    source << 
      2, 5, -2, 4,
      3, 2, 1,  0,
      -3, 4, 1, 1;

    numeric<float>::matrix expect(D * size, 4);
    expect <<
      0 ,  2,  5, -2,
      0 ,  3,  2,  1,
      0 , -3,  4,  1,
      2 ,  5, -2,  4,
      3 ,  2,  1,  0,
      -3,  4,  1,  1,
      5 , -2,  4,  0,
      2 ,  1,  0,  0,
      4 ,  1,  1,  0;

    node.set_size(size);
    node.set_shift(shift);
    node.set_left_padding(window_node<float>::padding_zero, 1);
    node.set_right_padding(window_node<float>::padding_zero, 1);

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

  TEST(window_node_test, RepeatPadding) {
    window_node<float> node("test", "window_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));

    int D = 3, T = 4, shift = 1, size = 3;
    numeric<float>::matrix source(D, T);
    source << 
      2, 5, -2, 4,
      3, 2, 1,  0,
      -3, 4, 1, 1;

    numeric<float>::matrix expect(D * size, 4);
    expect <<
      2 ,  2,  5, -2,
      3 ,  3,  2,  1,
      -3, -3,  4,  1,
      2 ,  5, -2,  4,
      3 ,  2,  1,  0,
      -3,  4,  1,  1,
      5 , -2,  4,  4,
      2 ,  1,  0,  0,
      4 ,  1,  1,  1;

    node.set_size(size);
    node.set_shift(shift);
    node.set_left_padding(window_node<float>::padding_repeat, 1);
    node.set_right_padding(window_node<float>::padding_repeat, 1);

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
