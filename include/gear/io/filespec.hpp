#ifndef gear_io_filespec_hpp_
#define gear_io_filespec_hpp_

namespace gear {
  inline infer_type_by_ext(const std::string& fn) {
    
  }

  struct file_spec {
    std::string path;
    std::string port;
    std::string type;
  };

  inline void parse_spec(file_spec& spec, std::string str, 
                         const std::string& def_port, 
                         const std::string& def_type,
                         bool infer_type_ext = true,
                         bool infer_type_content = true) {
    size_t idx;
    idx = str.rfind('@');
    if (idx == std::string::npos) {
      spec.port = def_port;
    } else {
      spec.port = str.substr(idx + 1);
      str.erase(idx);
    }
    
    idx = str.rfind(':');
    if (idx == std::string::npos) {
      spec.type = "";
      if (spec.type.size() == 0 && infer_type_ext) {
        spec.type = infer_type_by_ext(str);
      } 
      if (spec.type.size() == 0 && infer_type_content) {
        spec.type = infer_type_by_content(str);
      } 
      
      if (spec.type.size() == 0) {
        spec.type = def_type;
      }
    } else {
      spec.port = str.substr(idx + 1);
      str.erase(idx);
    }
    
    spec.path = str;
  }

}

#endif
