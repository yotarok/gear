#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/melfb.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(MelFB_nodeTest, RandomData) {
    MelFB_node node("test", "MelFB_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));
    
    int D = 512, T = 3;
    numeric<float>::matrix source = numeric<float>::matrix::Random(D, T);

    node.set_nchans(26);
    node.set_nyquist_hz(8000);

    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
    node.on_BOS(bospacket);

 
    vector_data_packet<float> packet(source);
    node.on_data(packet); 
    
    //std::cout << node.get_fbank_matrix() << std::endl;
  }
}
