#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/concat.hpp>
#include <gear/node/delta.hpp>
#include <gear/node/elemfunc.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(concat_node_test, monolithic) {
    concat_node<float> node("conc", "concat_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");
    node.set_nports(3);
    node.connect("", sink.input_port(""));

    std::vector<numeric<float>::matrix > sources;
    sources.push_back(numeric<float>::matrix(2, 4));
    sources.push_back(numeric<float>::matrix(3, 4));
    sources.push_back(numeric<float>::matrix(1, 4));
                                                     
    sources[0] << 
      2, 5, -2, 4,
      3, 2, 1,  0;
    sources[1] << 
      5, 2, -1, 3,
      5, 1, 1, 4,
      -2, -5, -7, -3;
    sources[2] << 
      0, 0, 1, 0;
    

    numeric<float>::matrix expect(6, 4);
    expect <<
      2, 5, -2, 4,
      3, 2, 1,  0,
      5, 2, -1, 3,
      5, 1, 1, 4,
      -2, -5, -7, -3,
      0, 0, 1, 0;

    for (int p = 0; p < 3; ++ p) {
      BOS_packet bospacket;
      bospacket.datainfo = 
        std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(sources[p].rows()));
      node.on_BOS(p, bospacket);

      vector_data_packet<float> packet(sources[p]);
      node.on_data(p, packet); 

      EOS_packet eospacket;
      node.on_EOS(p, eospacket); 
    }

    numeric<float>::matrix out = sink.get_output();
      
    assert_eq_matrix_Linf<float>(expect, out, 1e-8);

  }

  TEST(concat_node_test, D_A_concat) {
    vector_source_node<float> src("src", "vector_source_node");
    delta_node delta_D("delta", "delta_node");
    delta_node delta_A("delta", "delta_node");
    concat_node<float> conc_S_D_A("conc", "concat_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");
    
    conc_S_D_A.set_nports(3);
    delta_D.set_halfwidth(2);
    delta_A.set_halfwidth(2);

    src.connect("", conc_S_D_A.input_port("0"));
    src.connect("", delta_D.input_port(""));
    delta_D.connect("", conc_S_D_A.input_port("1"));
    delta_D.connect("", delta_A.input_port(""));
    delta_A.connect("", conc_S_D_A.input_port("2"));
    conc_S_D_A.connect("", sink.input_port(""));
    src.set_meta_data(2, 100, "");

    numeric<float>::matrix source(2, 12);
    source << 
      2, 5, -2, 4, 2, 4, 2, 5, -2, 4, 2, 4,
      3, 2, 1,  0, 2, 1, 3, 2, 1,  0, 2, 1;
    src.append_data(source);

    src.feed_BOS();
    src.feed_data_all();
    src.feed_EOS();
    
    numeric<float>::matrix out = sink.get_output();
    ASSERT_EQ(source.cols(), out.cols());
    ASSERT_EQ(source.rows() * 3, out.rows());
    std::cout << out << std::endl;
    //assert_eq_matrix_Linf<float>(expect, out, 1e-8);
  }
}

    
