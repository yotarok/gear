#include <gear/node/fft.hpp>

#include <unsupported/Eigen/FFT>
#include <gear/node/factory.hpp>

namespace gear {

  flow_node_ptr create_FFT_node(const YAML::Node& src) {
    FFT_node* ret = new FFT_node(src["name"].as<std::string>(),
                                 src["type"].as<std::string>());
    if (src["nbins"]) {
      ret->set_nbins(src["nbins"].as<int>());
    }
    if (src["power"]) {
      ret->set_nbins(src["power"].as<int>());
    }
    if (src["window"]) {
      ret->set_window(src["window"].as<std::string>());
    }
    return flow_node_ptr(ret);
  }
  
  static node_factory_registry FFT_node_registry("FFT", create_FFT_node);
  
  struct FFT_context {
    Eigen::FFT<float> impl;
  };
  
  FFT_node::FFT_node(const std::string& id, const std::string& type)
    : flow_node(id, type), _nbins(256), _power(1), _wintype(window_square) {
    _fft = new FFT_context;
    _fft->impl.SetFlag(Eigen::FFT<float>::Unscaled);
  }

  FFT_node::~FFT_node() {
    delete _fft;
  }

  void FFT_node::initialize_window_function()  {
    if (_wintype == window_square) {
      _window = numeric<float>::vector::Constant(_input_dims, 1.0f);
    }
    else if (_wintype == window_hamming) {
      _window = numeric<float>::vector::Zero(_input_dims);
      for (int t = 0; t < _input_dims; ++ t) {
        float x = static_cast<float>(t) / (_input_dims - 1);
        _window(t) = 0.54 - 0.46 * std::cos(2 * M_PI * x);
      }
    }
  }


  void FFT_node::set_window(const std::string& name) {
    if (name == "square") {
      set_window(window_square);
    } else if (name == "hamming") {
      set_window(window_hamming);
    } else {
      throw std::runtime_error("Window type [" + name + "] is not defined");
    }
  }

  void FFT_node::on_BOS(const BOS_packet& packet) {
    _input_dims = packet.get_info<vector_data_info<float> >().elems;
    BOS_packet npacket(new vector_data_info<float>(_nbins));
    _output(npacket);
    
    initialize_window_function();
  }
  
  void FFT_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;
    numeric<std::complex<float> >::vector outbuf(_nbins * 2);
    numeric<float>::vector inbuf(_nbins * 2);
    
    int T = packet.data.cols();
    vector_data_packet<float> npacket(numeric<float>::matrix::Zero(_nbins, T));
    
    for (int c = 0 ; c < T; ++ c) {
      inbuf.setZero();
      inbuf.head(packet.data.rows()) = 
        packet.data.col(c).cwiseProduct(_window);
      _fft->impl.fwd(outbuf, inbuf);
      
      npacket.data.col(c) = 
        outbuf.head(_nbins).array().abs().pow(_power).matrix();
    }
    _output(npacket);
    
  }


}
