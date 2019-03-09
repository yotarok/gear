#ifndef gear_flow_buffer_hpp_
#define gear_flow_buffer_hpp_

#include <gear/types.hpp>

namespace gear {

  template <typename ElemT>
  class buffer {
    typename numeric<ElemT>::matrix _ringbuf;
    int _readhead;
    int _writehead;
    int _ndim;
  public:
    buffer();

    void set_size(int ndim, int length, int shift) {
      _ringbuf.resize(ndim, length);
      _head = 0;
      _ndim = ndim;
    }

    void on_data(const VectorDataPacket<ElemT>& packet) {
      int readhead = 0;
      
      _rigpacket.data.block(0, readhead, _ndim, readlen);
    }
  };

}

#endif
