#include <gtest/gtest.h>

#include <gear/io/logging.hpp>
#include <gear/io/fileformat.hpp>

namespace {
  using namespace gear;

  TEST(content_type_test, parsing) {
    {
      content_type ct("audio/htk");
      ASSERT_EQ(ct.type(), "audio/htk");
      ASSERT_EQ(ct.get_prop("endian", "LE"), "LE");
      ASSERT_EQ(ct.get_prop("not_defined", "default"), "default");
    }
    {
      content_type ct(" audio/htk; endian=BE \t ");
      ASSERT_EQ(ct.type(), "audio/htk");
      ASSERT_EQ(ct.get_prop("endian", "LE"), "BE");
      ASSERT_EQ(ct.get_prop("not_defined", "default"), "default");
    }
    {
      content_type ct(" audio/htk; endian=BE \t;");
      ASSERT_EQ(ct.type(), "audio/htk");
      ASSERT_EQ(ct.get_prop("endian", "LE"), "BE");
      ASSERT_EQ(ct.get_prop("not_defined", "default"), "default");
    }
  }

}
