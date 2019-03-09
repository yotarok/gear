#ifndef gear_tool_main_hpp_
#define gear_tool_main_hpp_

#include <tclap/CmdLine.h>
#include <boost/function.hpp>

namespace gear {
  template <typename ArgType>
  int wrap_main(const char* tool_descr, int argc, char* argv[],
                int (*main) (ArgType&, int, char*[])) {
    TCLAP::CmdLine cmdline(tool_descr);
    ArgType arg;
    arg.add(cmdline);
    cmdline.parse(argc, argv);
    try {
      int ret = main(arg, argc, argv);
      return ret;
    } catch (std::exception& e) {
      FATAL("Caught %s", e.what());
      return -1;
    } catch (std::exception* pe) {
      FATAL("Caught %s", pe->what());
      return -1;
    } catch (...) {
      FATAL("Unknown error");
      return -1;
    }
  }
}

#endif
