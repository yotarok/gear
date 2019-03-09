#ifndef gear_node_melfb_hpp_
#define gear_node_melfb_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <gear/node/affinetrans.hpp>

#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>

namespace gear {
  class MelFB_node : public affine_transform_node {
    int _nchans;

    int _nyquist_hz;
    size_t _input_dims;

    float hz2mel(float hz) {
      return 1127.0 * std::log(1.0 + hz / 700.0);
    }
    float mel2hz(float mel) {
      return (std::exp(mel / 1127.0) - 1.0) * 700;
    }
    float mel2bin(float mel) {
      return (mel2hz(mel) / (_nyquist_hz) * (_input_dims));
    }
    float bin2mel(size_t bin) {
      return hz2mel(static_cast<float>(bin) / static_cast<float>(_input_dims - 1) * _nyquist_hz);
    }

  
  public:

    MelFB_node(const std::string& id, const std::string& type)
      : affine_transform_node(id, type), _nchans(26), _nyquist_hz(8000) {
    }

    const numeric<float>::matrix& get_fbank_matrix() const { // for debug
      return _weight;
    }

    void set_nchans(size_t nc) { _nchans = nc; }
    void set_nyquist_hz(int nq) { _nyquist_hz = nq; }

    void initialize_fbank ();
    
    void on_BOS(const BOS_packet& packet) {
      _input_dims = packet.get_info<vector_data_info<float> >().elems;
      BOS_packet npacket(new vector_data_info<float>(_nchans));
      initialize_fbank();
      _output(npacket);
    }


    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      packet_handler_ptr p = affine_transform_node::input_port(portname);
      std::dynamic_pointer_cast<packet_handler_chain>(p)
        ->override<BOS_packet>(this, &MelFB_node::on_BOS);
      return p;
    }
    
    virtual flow_node_ptr clone() const {
      MelFB_node* ret = new MelFB_node(get_id(), get_type());
      ret->set_nchans(_nchans);
      ret->set_nyquist_hz(_nyquist_hz);
      return flow_node_ptr(ret);
    }


  };
}

#endif
