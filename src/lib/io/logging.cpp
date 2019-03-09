#include <gear/io/logging.hpp>

#include <iostream>
#include <ctime>
#include <cstdarg>
#include <cstdio>

#define LOGBUFFER_SIZE 4096

namespace gear {
  logger* logger::_the_instance = 0;
  
  logger& logger::get_instance() {
    if (logger::_the_instance == 0) { 
      logger::_the_instance = new logger;
    }
    return *_the_instance;
  }

  static char _g_log_buffer[LOGBUFFER_SIZE];

  static const char* _g_log_level_names[] = {
    "FATAL", "ERROR", "WARN", "INFO", "DEBUG", "TRACE",
  };

  const char* format_log_entry(loglevel lv, const char* file_name,
                               int linum, const char* func_name, 
                               const char* fmt, ...) {
    std::time_t rawtime = time(0);
    std::string basename = file_name;
    basename.erase(0, basename.rfind('/') + 1);
    
    std::tm* time = std::localtime(&rawtime);
    int p = 0, n = 0;

    n = std::snprintf(_g_log_buffer + p, LOGBUFFER_SIZE - p - 1,
                      "%c %04d-%02d-%02dT%02d:%02d:%02d %16s:%3d| ", 
                      _g_log_level_names[lv][0], 
                      time->tm_year + 1900, time->tm_mon + 1, time->tm_mday,
                      time->tm_hour, time->tm_min, time->tm_sec, 
                      basename.c_str(), linum);
    p += n;
                  
    std::va_list args;
    va_start(args, fmt);
    n = std::vsnprintf(_g_log_buffer + p, LOGBUFFER_SIZE - p - 1, fmt, args);
    p += n;
    va_end(args);
    if (n < 0) return "error in formatlogEntry";

    // add EOL
    _g_log_buffer[p] = '\n';
    _g_log_buffer[p+1] = '\0';

    return _g_log_buffer;
  }

  void logger::write(loglevel lv, const char* str) {
    if (lv <= _log_level) {
      std::cerr << str;
      //std::printf(str); // for 
    }
  }
}
