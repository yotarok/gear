#ifndef gear_test_utils_hpp_
#define gear_test_utils_hpp_

#include <gtest/gtest.h>
#include <gear/types.hpp>

#include <boost/format.hpp>

namespace gear {
  template<typename NumT>
  void assert_eq_matrix_Linf(const typename numeric<NumT>::matrix& expect,
                           const typename numeric<NumT>::matrix& actual,
                           double torrelance) {
    SCOPED_TRACE("assert_eq_matrix_L1");
    ASSERT_EQ(expect.rows(), actual.rows());
    ASSERT_EQ(expect.cols(), actual.cols());

    for (int col = 0; col < expect.cols(); ++ col) { 
      for (int row = 0; row < expect.rows(); ++ row) {
        SCOPED_TRACE((boost::format("row = %d, col = %d") % row % col).str());
        NumT diff = std::abs(expect(row, col) - actual(row, col));
        ASSERT_NEAR(0, diff, torrelance);
      }
    }
  }
}

#endif
