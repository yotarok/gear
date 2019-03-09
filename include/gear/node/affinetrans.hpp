#ifndef gear_node_affinetrans_hpp_
#define gear_node_affinetrans_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>

namespace gear {

  class affine_transform_node : public flow_node {

  protected:
    packet_handler_set _output;

    bool _has_weight;
    numeric<float>::matrix _weight;
    bool _has_bias;
    numeric<float>::vector _bias;
    bool _has_scale;    
    numeric<float>::vector _scale;

    int _outdims;

  public:
    affine_transform_node(const std::string& id, const std::string& type)
      : flow_node(id, type), _has_weight(false), _has_bias(false),
        _has_scale(false) {
    }

    const numeric<float>::matrix& weight() const { return _weight; }
    const numeric<float>::vector& bias() const { return _bias; }
    const numeric<float>::vector& scale() const { return _scale; }

    void set_weight(const numeric<float>::matrix& w);

    void set_scale(const numeric<float>::vector& s);
    
    void set_bias(const numeric<float>::vector& b);

    void on_data(const vector_data_packet<float>& packet);
    
    virtual packet_handler_ptr input_port(const std::string& portname);


    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest);

    // Make clone that doesn't have connection
    virtual flow_node_ptr clone() const;

  };

  class DCT_node : public affine_transform_node {
    int _ncoefs;
    int _offset;
  public:
    void set_ncoefs(int n) { _ncoefs = n; }
    void set_offset(int n) { _offset = n; }

    DCT_node(const std::string& id, const std::string& type)
      : affine_transform_node(id, type) {
    }

    void on_BOS(const BOS_packet& packet) {
      int D = packet.get_info<vector_data_info<float> >().elems;
      
      _outdims = _ncoefs;
      _weight = numeric<float>::matrix::Zero(_ncoefs, D);
      float pi_by_D = M_PI / static_cast<float>(D);
      for (int r = 0; r < _ncoefs; ++ r) {
        for (int c = 0; c < D; ++ c) {
          _weight(r, c) = std::cos(static_cast<float>(r + _offset) * pi_by_D *
                                   (static_cast<float>(c) + 0.5))
            * std::sqrt(2.0f / D);
        }
      }
      _has_weight = true;

      BOS_packet npacket(new vector_data_info<float>(_ncoefs));
      _output(npacket);
    }


    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      packet_handler_ptr p = affine_transform_node::input_port(portname);
      std::dynamic_pointer_cast<packet_handler_chain>(p)
        ->override<BOS_packet>(this, &DCT_node::on_BOS);
      return p;
    }

    virtual flow_node_ptr clone() const {
      DCT_node* ret = new DCT_node(get_id(), get_type());
      ret->set_ncoefs(_ncoefs);
      ret->set_offset(_offset);
      return flow_node_ptr(ret);
    }
    
  };

  


}
    
  
#endif
