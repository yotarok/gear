#include <gear/node/noise.hpp>
#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_Gaussian_noise_node(const YAML::Node& src) {
    Gaussian_noise_node* ret =
      new Gaussian_noise_node(src["name"].as<std::string>(),
                              src["type"].as<std::string>());
    if (src["stdev"]) {
      ret->set_stdev(src["stdev"].as<float>());
    }
    if (src["seed"]) {
      ret->set_seed(src["seed"].as<int>());
    }
    return flow_node_ptr(ret);
  }
  static node_factory_registry
  Gaussian_noise_node_registry("Gaussian_noise", create_Gaussian_noise_node);

  void Gaussian_noise_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;
    int T = packet.data.cols();
    int D = packet.data.rows();
    
    numeric<float>::vector stdev = (_stdev.which() == 0)
      ? numeric<float>::vector::Constant(D, boost::get<float>(_stdev))
      : boost::get<numeric<float>::vector>(_stdev);
    
    numeric<float>::matrix ndata = packet.data;
    for (int t = 0; t < T; ++ t) {
      for (int d = 0; d < D; ++ d) {
        ndata(d, t) += _norm_dist(_rng) * stdev(d);
      }
    }
    
    _output(vector_data_packet<float>(ndata));
  }

  

}


