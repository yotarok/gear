#ifndef gear_data_binary_hpp_
#define gear_data_binary_hpp_

#include <gear/types.hpp>

namespace gear {
  //! swap endian of N-byte binary data
  template <size_t N> void swap(char* b) {
    std::swap(b[0], b[N-1]);
    swap<N-2>(b+1);
  }

  template <>
  inline void swap<1>(char* b) {  }

  template <>
  inline void swap<0>(char* b) {  }

  template <typename NumT>
  inline void write_scalar(std::ostream& os, NumT n, bool is_BE) {
    char buf[sizeof(NumT)];
    std::memcpy(buf, &n, sizeof(NumT));

#ifdef SLOPE_NATIVE_IS_BE
    if (! is_BE) swap<sizeof(NumT)>(buf);
#else
    if (is_BE) swap<sizeof(NumT)>(buf);
#endif

    os.write(buf, sizeof(NumT));
  }

  template <typename NumT>
  inline NumT read_scalar(std::istream& is, bool is_BE) {
    char buf[sizeof(NumT)];
    is.read(buf, sizeof(NumT));

#ifdef SLOPE_NATIVE_IS_BE
    if (! is_BE) swap<sizeof(NumT)>(buf);
#else
    if (is_BE) swap<sizeof(NumT)>(buf);
#endif

    return *reinterpret_cast<NumT*>(buf);
  }

}

#endif
