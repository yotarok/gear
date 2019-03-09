#ifndef gear_tool_args_hpp_
#define gear_tool_args_hpp_

#include <tclap/CmdLine.h>

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor.hpp>
#include <boost/preprocessor/punctuation.hpp>
#include <boost/preprocessor/repetition/repeat.hpp>

#define DEFINE_EACH(r, data, i, x)                                      \
  BOOST_PP_TUPLE_ELEM(0, x) BOOST_PP_TUPLE_ELEM(1, x);                  
#define INIT_EACH(r, data, i, x)                                        \
 BOOST_PP_COMMA_IF(i) BOOST_PP_TUPLE_ELEM(1, x) BOOST_PP_TUPLE_ELEM(2, x)
#define EXTEND_EACH(r, data, i, x)                                      \
 BOOST_PP_COMMA_IF(i) x
#define ADD_EACH(r, data, i, x)                 \
  cmdline.add( BOOST_PP_TUPLE_ELEM(1, x) ) ;
#define ADD_SUPER_EACH(r, data, i, x)           \
  x :: add(cmdline);

#define DEFINE_ARGCLASS(name, supers, ...)                              \
  struct name :                                                         \
    BOOST_PP_SEQ_FOR_EACH_I(EXTEND_EACH, "", BOOST_PP_TUPLE_TO_SEQ(supers)) \
  {                                                                     \
    name() :                                                             \
      BOOST_PP_SEQ_FOR_EACH_I(INIT_EACH, "", BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
      {}                                                                \
    BOOST_PP_SEQ_FOR_EACH_I(DEFINE_EACH, "", BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)) \
      virtual void add(TCLAP::CmdLine& cmdline) {                        \
      BOOST_PP_SEQ_FOR_EACH_I(ADD_SUPER_EACH, "", BOOST_PP_TUPLE_TO_SEQ(supers)); \
      BOOST_PP_SEQ_FOR_EACH_I(ADD_EACH, "", BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)); \
    }                                                                   \
  }

namespace gear {
  struct base_args {
    virtual void add(TCLAP::CmdLine& cmdline) {
    }
  };

  struct common_args {
    TCLAP::MultiSwitchArg verbose;
    common_args() : verbose("v", "verbose", "verbose") {
    }
    virtual void add(TCLAP::CmdLine& cmdline) {
      cmdline.add(verbose);
    }
  };
}

#endif
