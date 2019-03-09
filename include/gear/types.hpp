#ifndef gear_types_hpp_
#define gear_types_hpp_

#include <Eigen/Eigen>

namespace gear {
  template <typename NumT>
  struct numeric {
    typedef NumT scalar;
    typedef Eigen::Matrix<NumT, Eigen::Dynamic, 1> vector;
    typedef Eigen::Array<NumT, Eigen::Dynamic, 1> array1d;
    typedef Eigen::Matrix<NumT, Eigen::Dynamic, Eigen::Dynamic> matrix;
    typedef Eigen::Array<NumT, Eigen::Dynamic, Eigen::Dynamic> array2d;
  };
}

#endif
