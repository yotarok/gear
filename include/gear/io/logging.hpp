#ifndef gear_io_logging_hpp_
#define gear_io_logging_hpp_

#include <string>

namespace gear {
  enum loglevel {
    loglevel_FATAL = 0,
    loglevel_ERROR = 1,
    loglevel_WARN  = 2,
    loglevel_INFO  = 3,
    loglevel_DEBUG = 4,
    loglevel_TRACE = 5
  };

  class logger {
    static logger* _the_instance;    
  public:
    loglevel _log_level = loglevel_TRACE; // Initial log level is set to high
    static logger& get_instance();
    void write(loglevel lv, const char* str);
  };

  const char* format_log_entry(loglevel lv, const char* fileName,
                               int linum, const char* funcName, 
                               const char* fmt, ...);
     

#define FATAL(...)                                                      \
  gear::logger::get_instance()                                         \
  .write(gear::loglevel_FATAL,                                         \
           gear::format_log_entry(gear::loglevel_FATAL, __FILE__, __LINE__, \
                                     __func__, __VA_ARGS__))            

#define ERROR(...)                                                      \
  gear::logger::get_instance()                                          \
  .write(gear::loglevel_ERROR,                                         \
         gear::format_log_entry(gear::loglevel_ERROR, __FILE__, __LINE__, \
                                 __func__, __VA_ARGS__))            

#define WARN(...)                                                       \
  gear::logger::get_instance()                                         \
  .write(gear::loglevel_WARN,                                          \
         gear::format_log_entry(gear::loglevel_WARN, __FILE__, __LINE__, \
                                 __func__, __VA_ARGS__))            
 
#define INFO(...)                                                       \
  gear::logger::get_instance()                                         \
  .write(gear::loglevel_INFO,                                          \
         gear::format_log_entry(gear::loglevel_INFO, __FILE__, __LINE__, \
                                 __func__, __VA_ARGS__))            
#ifdef ENABLE_TRACE

#  define TRACE(...)                                                    \
  gear::logger::get_instance()                                         \
  .write(gear::loglevel_TRACE,                                         \
         gear::format_log_entry(gear::loglevel_TRACE, __FILE__, __LINE__, \
                                 __func__, __VA_ARGS__))            

#  define TRACE_RAW(s)                                                  \
  gear::logger::get_instance()                                         \
  .write(gear::loglevel_TRACE, s)    

#else
#  define TRACE(...)
#  define TRACE_RAW(...)
#endif

}

#endif
