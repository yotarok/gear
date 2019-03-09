#ifndef gear_vectordeque_hpp_
#define gear_vectordeque_hpp_

#include <gear/types.hpp>

namespace gear {
  template <typename NumT>
  class vector_deque {
    typename numeric<NumT>::matrix _data;
    int _dims;
    size_t _length; // actual length

  public:
    vector_deque() : _dims(-1) {
    }

    vector_deque(size_t D, size_t initsize = 4)
      : _dims(D), _data(D, initsize), _length(0) {
    }

    void push_back(const typename numeric<NumT>::matrix& data) {
      if (_dims < 0) throw std::runtime_error("VectorDeque is not initialized");
      size_t nsize = _data.cols();
      while (nsize < _length + data.cols()) nsize *= 2;
      if (nsize != _data.cols()) {
        TRACE("Extended buffer size from (%d, %d) to (%d, %d)", 
              _data.rows(), _data.cols(), _dims, nsize);
        _data.conservativeResize(_dims, nsize);
      }
      
      TRACE("Replace block (%d, %d, %d, %d) of (%d, %d) matrix with data (%d, %d)",
            0, _length, _dims, data.cols(), _data.rows(), _data.cols(), data.rows(), data.cols());

      _data.block(0, _length, _dims, data.cols()) = data;
      _length += data.cols();
    }
    
    int ndims() const { return _dims; }
    size_t size() const { return _length; }
    
    typename numeric<NumT>::vector at(size_t t) const {
      return _data.col(t);
    }

    typename numeric<NumT>::matrix all() const {
      return _data.block(0, 0, _dims, _length);
    }

    void pop_front(size_t n) {
      if (_dims < 0) throw std::runtime_error("VectorDeque is not initialized");
      typename numeric<NumT>::matrix mov = 
        _data.block(0, n, _dims, _length - n);
      _data.block(0, 0, _dims, mov.cols()) = mov;
      _length -= n;
    }

  };
}

#endif
