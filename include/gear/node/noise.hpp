#ifndef gear_node_noise_hpp_
#define gear_node_noise_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <boost/format.hpp>
#include <boost/variant.hpp>
#include <boost/random.hpp>

namespace gear {
  class Gaussian_noise_node : public flow_node {
  private:
    boost::variant<float,
                   numeric<float>::vector> _stdev;

    int _seed; // saved for cloning
    boost::random::mt19937 _rng;
    boost::random::normal_distribution<> _norm_dist;

    packet_handler_set _output;

  public:
    Gaussian_noise_node(const std::string& id, const std::string& type)
      : flow_node(id, type), _stdev(1.0f), _rng(0), _norm_dist() {
    }

    void set_stdev(float f) {
      _stdev = f;
    }

    void set_stdev(const numeric<float>::vector& v) {
      _stdev = v;
    }

    void set_seed(int seed) {
      _seed = seed;
      _rng.seed(seed);
    }

    void on_data(const vector_data_packet<float>& packet);
    
    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      
      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->add<vector_data_packet<float> >(this, &Gaussian_noise_node::on_data)
                           ->add_thru<control_packet>(&this->_output)
                           );
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      auto ret = new Gaussian_noise_node(get_id(), get_type());
      ret->set_seed(_seed);
      if (_stdev.which() == 0) {
        ret->set_stdev(boost::get<float>(_stdev));
      } else {
        ret->set_stdev(boost::get<numeric<float>::vector>(_stdev));
      }
      return flow_node_ptr(ret);
    }

  };
}

#endif
