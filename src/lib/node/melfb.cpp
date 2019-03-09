#include <gear/node/melfb.hpp>

#include <gear/node/factory.hpp>

namespace gear {
  flow_node_ptr create_MelFB_node(const YAML::Node& src) {
    MelFB_node* ret = new MelFB_node(src["name"].as<std::string>(),
                                     src["type"].as<std::string>());
    if (src["nyquist_hz"]) {
      ret->set_nyquist_hz(src["nyquist_hz"].as<int>());
    }
    if (src["nchans"]) {
      ret->set_nchans(src["nchans"].as<int>());
    }
    return flow_node_ptr(ret);
  }

  static node_factory_registry MelFB_node_registry("MelFB", create_MelFB_node);

  void MelFB_node::initialize_fbank () {
    _has_weight = true;
    _outdims = _nchans;
    int klo = 0, khi = _input_dims;
    float hzlo = static_cast<float>(klo) / _input_dims * _nyquist_hz, 
      hzhi = static_cast<float>(khi) / _input_dims * _nyquist_hz;
    TRACE("Bank width = [%f, %f] (Hz)", hzlo, hzhi);
    float mlo = hz2mel(hzlo), mhi = hz2mel(hzhi), mfullwid = mhi - mlo;
    TRACE("Bank width = [%f, %f] (mel)", mlo, mhi);

    std::vector<float> mchcents;
    
    TRACE_RAW("Central frequencies = [");
    for (int ch = 0; ch <= _nchans + 1; ++ ch) { // include high-cutoff boundary
      mchcents.push_back(mfullwid / (_nchans + 1.0f) * ch + mlo);
      TRACE_RAW((boost::format("%f, ") % mchcents[mchcents.size() - 1]).str().c_str());
    }
    TRACE_RAW("]\n");
    
    /*
      std::vector<int> leftchs;
      int curch = 1;
      TRACE_RAW("Left channel IDs = [");
      for (int k = 0; k < _input_dims; ++ k) {
      if (k < klo || k > khi) {
      leftchs.push_back(-2);
      TRACE_RAW("-2, ");
      } else {
      float m = bin2mel(k);
      while (mchcents[curch] < m && curch <= _nchans) ++ curch;
      
          leftchs.push_back(curch - 2);
          TRACE_RAW((boost::format("%d, ") % (curch - 2)).str().c_str());
          }
          }
          TRACE_RAW("]\n");
    */
    
    _weight = numeric<float>::matrix::Zero(_nchans, _input_dims);
    for (int ch = 0; ch <= _nchans; ++ ch) {
      for (int k = klo; k <= khi; ++ k) {
        float m = bin2mel(k);
        float rightW = (m - mchcents[ch]) / (mchcents[ch + 1] - mchcents[ch]);
        if (rightW <= 0.0 || rightW > 1.0) continue;
        if (ch < _nchans) _weight(ch, k) = rightW;
        if (ch > 0) _weight(ch - 1, k) = 1.0 - rightW;
      }
    }
    _outdims = _nchans;
  }
  
  
}
