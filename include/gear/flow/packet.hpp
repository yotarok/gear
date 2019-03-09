#ifndef gear_flow_packet_hpp_
#define gear_flow_packet_hpp_

#include <gear/types.hpp>
#include <memory>

namespace gear {

  class packet {
  public:
    virtual ~packet() { }
  };

  struct data_info {
    virtual ~data_info() { }
    //std::string datatype;
    int32_t bytes_per_frame; // -1 indicates variable length
  };

  template <typename T>
  struct vector_data_info : public data_info {
    int32_t elems;
    vector_data_info(int32_t d) {
      //this->datatype = VectorDataPacket<T>::type();
      reset_elems(d);
    }
    void reset_elems(int32_t d) {
      elems = d;
      bytes_per_frame = sizeof(T) * d;
    }
  };

  class control_packet : public packet {
  };

  struct BOS_packet : public control_packet {
    std::shared_ptr<data_info> datainfo;
    BOS_packet() { }
    BOS_packet(data_info* i) : datainfo(i) { }
    template <typename T>
    const T& get_info() const {
      const T* p = dynamic_cast<const T*>(datainfo.get());
      return *p;
    }
  };

  class EOS_packet : public control_packet {
  };

  class data_packet : public packet {
  };

  // scalar is designed to be represented as 1-dimensional vector
  template <typename T>
  class vector_data_packet : public data_packet {
  public:
    typename numeric<T>::matrix data;
    vector_data_packet(const typename numeric<T>::matrix& m) : data(m) { }
  };


}

#endif

