#ifndef gear_node_htkcompat_hpp_
#define gear_node_htkcompat_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>
#include <gear/node/affinetrans.hpp>

namespace gear {
  class HTK_preemph_node : public flow_node {
    float _coef;
    packet_handler_set _output;

  public:

    HTK_preemph_node(const std::string& id, const std::string& type);

    void set_coef(float v) { _coef = v; }

    void on_data(const vector_data_packet<float>& packet);

    virtual packet_handler_ptr input_port(const std::string& portname);

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest);

    virtual flow_node_ptr clone() const;
    
  };

  class HTK_cepstral_scaler_node : public affine_transform_node {
    int _offset;
    int _const;
  public:
    HTK_cepstral_scaler_node(const std::string& id, const std::string& type) : affine_transform_node(id, type), _offset(1), _const(22) {
    }

    void set_offset(int n) { _offset = n; }
    void set_const(int n) { _const = n; }

    void on_BOS(const BOS_packet& packet) {
      int D = packet.get_info<vector_data_info<float> >().elems;
      _scale = numeric<float>::vector::Zero(D);
      _has_scale = true;

      float fac = M_PI / static_cast<float>(_const);
      for (int d = 0; d < D; ++ d) {
        _scale(d) = 1.0f + 
          (static_cast<float>(_const) / 2.0) * std::sin((d + _offset) * fac);
        
      }
      
      _output(packet); // changes nothing
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0)
        throw port_not_defined(*this, portname);
      
      packet_handler_ptr p = affine_transform_node::input_port(portname);
      std::dynamic_pointer_cast<packet_handler_chain>(p)
        ->override<BOS_packet>(this, &HTK_cepstral_scaler_node::on_BOS);
      return p;
    }

    virtual flow_node_ptr clone() const {
      HTK_cepstral_scaler_node* ret =
        new HTK_cepstral_scaler_node(get_id(), get_type());
      ret->set_offset(_offset);
      ret->set_const(_const);
      return flow_node_ptr(ret);
    }
  };

}
#endif
