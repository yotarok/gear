#include <gtest/gtest.h>

#include <gear/types.hpp>
#include <gear/math/basics.hpp>
#include <gear/io/logging.hpp>
#include <gear/node/fft.hpp>
#include <gear/test/utils.hpp>

namespace {
  using namespace gear;

  TEST(FFT_nodeTest, WithoutPadding) {
    FFT_node node("test", "FFT_node");
    vector_sink_node<float> sink("sink", "vector_sink_node<float>");

    node.connect("", sink.input_port(""));

    int D = 3, T = 4, nbins = 4;
    numeric<float>::matrix source(D, T);
    source << 
      2, 5, -2, 4,
      3, 2, 1,  0,
      -3, 4, 1, 1;

    numeric<float>::matrix expect(nbins, T);

    numeric<std::complex<float> >::matrix dftmat(nbins*2, nbins*2);
    for (int j = 0; j < dftmat.rows(); ++ j) {
      for (int i = 0; i < dftmat.cols(); ++ i) {
        dftmat(i, j) = std::complex<float>(std::cos(- 2 * M_PI * i * j / (nbins*2)),
                                           std::sin(- 2 * M_PI * i * j / (nbins*2)));
      }
    }
    for (int t = 0; t < T; ++ t) {
      numeric<float>::vector inp = numeric<float>::vector::Zero(nbins * 2);
      inp.head(source.rows()) = source.col(t);
      expect.col(t) = (dftmat * inp).head(nbins).array().abs().matrix();
    }


    node.set_nbins(nbins);

    BOS_packet bospacket;
    bospacket.datainfo = std::shared_ptr<vector_data_info<float> >(new vector_data_info<float>(D));
    node.on_BOS(bospacket);


    vector_data_packet<float> packet(source);
    node.on_data(packet); 

    numeric<float>::matrix out = sink.get_output();

    assert_eq_matrix_Linf<float>(expect, out, 1e-6);
  }
}
