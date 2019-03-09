#ifndef gear_node_window_hpp_
#define gear_node_window_hpp_

#include <gear/node/node.hpp>
#include <gear/flow/packet.hpp>
#include <memory>
#include <gear/io/logging.hpp>
#include <gear/vectordeque.hpp>
#include <iostream>

namespace gear {

  template <typename NumT>
  class window_node : public flow_node {
  public:
    enum padding_type {
      padding_repeat,
      padding_zero,
      padding_drop 
    };
  private:

    packet_handler_set _output;

    int _left_padding;
    padding_type _left_padding_type;
    int _right_padding;
    padding_type _right_padding_type;

    bool _first;
    typename numeric<NumT>::vector _lastdata; 

  protected:
    int _size;
    int _shift;
    int _input_dims;

    vector_deque<NumT> _buffer;

    virtual typename numeric<NumT>::matrix setup_output_data() {
      int nlen = (_buffer.size() - _size) / _shift + 1;

      typename numeric<NumT>::matrix ndata(_input_dims * _size, nlen);
      for (int i = 0; i < nlen; ++ i) {
        for (int t = 0; t < _size; ++ t) {
          ndata.col(i).segment(_input_dims * t, _input_dims) = 
            _buffer.at(_shift * i + t);
        }
      }
      return ndata;
    }

    void flush_buffer() {      
      if (_buffer.size() >= _size) {
        typename numeric<NumT>::matrix ndata = this->setup_output_data();
        _buffer.pop_front(_shift * ndata.cols());

        vector_data_packet<NumT> npacket(ndata);
        _output(npacket);
      }
    }

  public:

    window_node(const std::string& id, const std::string& type)
      : flow_node(id, type), _size(1), _shift(1), 
        _left_padding(0), _left_padding_type(padding_drop),
        _right_padding(0), _right_padding_type(padding_drop), _input_dims(-1) { }

    void set_size(int size) {
      _size = size;
    }

    void set_shift(int shift) { _shift = shift; }

    padding_type get_padding_type(const std::string& s) {
      if (s == "repeat") return padding_repeat;
      else if (s == "zero") return padding_zero;
      else if (s == "drop") return padding_drop;
      else {
        throw std::runtime_error("Unknown padding type: " + s);
      }
    }

    void set_left_padding(const std::string& t, int n) {
      set_left_padding(get_padding_type(t), n);
    }
    void set_left_padding(padding_type t, int n) { 
      _left_padding_type = t;
      _left_padding = n; 
    }
    void set_right_padding(const std::string& t, int n) {
      set_right_padding(get_padding_type(t), n);
    }
    void set_right_padding(padding_type t, int n) {
      _right_padding_type = t;
      _right_padding = n;
    }


    void on_data(const vector_data_packet<NumT>& packet) {

      if (packet.data.cols() == 0) return;

      if (_first) {
        _first = false;
        if (_left_padding_type == padding_zero) {
          TRACE("Padding %d zeros", _input_dims * _left_padding);
          auto zeros = numeric<NumT>::matrix::Zero(_input_dims, _left_padding);
          _buffer.push_back(zeros);
        }
        else if (_left_padding_type == padding_repeat) {
          for (int i = 0; i < _left_padding; ++ i) 
            _buffer.push_back(packet.data.col(0));
        }
      }

      if (_right_padding_type == padding_repeat) 
        _lastdata = packet.data.col(packet.data.cols() - 1);

      _buffer.push_back(packet.data);

      flush_buffer();
    }

    virtual size_t output_dims() {
      return _input_dims * _size;
    } 

    virtual void initialize(const BOS_packet& packet) {
    }

    void on_BOS(const BOS_packet& packet) {
      _first = true;
      _input_dims = packet.get_info<vector_data_info<NumT> >().elems;

      _buffer = vector_deque<NumT>(_input_dims);
      
      this->initialize(packet);

      BOS_packet npacket(new vector_data_info<NumT>(this->output_dims()));
      _output(npacket);
    }

    void on_EOS(const EOS_packet& packet) {

      if (_right_padding_type == padding_zero) {
        TRACE("Padding %d zeros", _input_dims * _right_padding);
        auto zeros = numeric<NumT>::matrix::Zero(_input_dims, _right_padding);
        _buffer.push_back(zeros);
        TRACE("pushed");
      }
      else if (_right_padding_type == padding_repeat && ! _first) {
        for (int t = 0; t < _right_padding; ++ t) 
          _buffer.push_back(_lastdata);
      }
      flush_buffer();
      _output(packet);
    }

    virtual packet_handler_ptr input_port(const std::string& portname) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);

      return 
        packet_handler_ptr((new packet_handler_chain)
                           ->template add<vector_data_packet<NumT> >(this, &window_node::on_data)
                           ->template add<BOS_packet>(this, &window_node::on_BOS)
                           ->template add<EOS_packet>(this, &window_node::on_EOS)
                           ->template add_thru<control_packet>(&this->_output)
                           );
    }

    virtual void connect(const std::string& portname,
                         packet_handler_ptr dest) {
      if (portname.size() != 0) throw port_not_defined(*this, portname);
      _output.insert(dest);
    }

    virtual flow_node_ptr clone() const {
      auto ret = new window_node<NumT>(get_id(), get_type());
      ret->set_size(_size);
      ret->set_shift(_shift);
      ret->set_left_padding(_left_padding_type, _left_padding);
      ret->set_right_padding(_right_padding_type, _right_padding);
      
      return flow_node_ptr(ret);
    }

  };

}

#endif
