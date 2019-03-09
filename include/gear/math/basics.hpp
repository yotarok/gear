#ifndef gear_math_basics_hpp_
#define gear_math_basics_hpp_

#include <gear/types.hpp>

namespace gear {
  template <typename NumT>
  typename numeric<NumT>::vector flatten(const typename numeric<NumT>::matrix& m) {
    typename numeric<NumT>::vector v(m.size());
    for (int c = 0; c < m.cols(); ++ c) {
      v.segment(m.rows() * c, m.rows()) = m.col(c);
    }
    return v;
  }
}

#endif
