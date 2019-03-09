#ifndef gear_node_fft_hpp_
#define gear_node_fft_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>


namespace gear {
  struct FFT_context;
  
  class FFT_node : public flow_node {
    int _nbins;
    int _power;
    int _input_dims;

    FFT_context* _fft;
    // ^ Hide FFT context so that client code doesn't need to rely
    //   on unsupported Eigen code
    
    numeric<float>::vector _window;

    packet_handler_set _output;

  public:
    enum window_function {
      window_square,
      window_hamming
    };
  private:
    window_function _wintype;
    void initialize_window_function();
    
  public:
    FFT_node(const std::string& id, const std::string& type);
    virtual ~FFT_node();
    
    void set_nbins(int n) { _nbins = n; }

    void set_power(int p) { _power = p; }

    void set_window(const std::string& name);
    void set_window(window_function ft) { _wintype = ft; }

    void on_data(const vector_data_packet<float>& packet);
    
    void on_BOS(const BOS_packet& packet);
    
    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      
      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->add<vector_data_packet<float> >(this, &FFT_node::on_data)
                           ->add<BOS_packet>(this, &FFT_node::on_BOS)
                           ->add_thru<control_packet>(&this->_output)
                           );
    }


    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      FFT_node* ret = new FFT_node(get_id(), get_type());
      ret->set_nbins(_nbins);
      ret->set_power(_power);
      ret->set_window(_wintype);
      return flow_node_ptr(ret);
    }


  };
}
#endif
