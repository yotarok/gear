#include <gear/io/fileformat.hpp>

#include <boost/algorithm/string.hpp>

namespace gear {
  content_type::content_type(const std::string& s) : _type(), _props() {
    parse_and_assign(s);
  }

  void content_type::parse_and_assign(const std::string& s) {
    size_t sep = s.find(';');
    if (sep == std::string::npos) {
      _type = s;
      boost::trim(_type);
    } else {
      _type = s.substr(0, sep);
      boost::trim(_type);
      
      // parse properties
      std::string buf = s.substr(sep+1);
      boost::trim(buf);
      while (buf.size() > 0) {
        sep = buf.find(';');

        std::string pair = buf.substr(0, sep);
        size_t eq = pair.find('=');
        if (eq == std::string::npos) {
          throw std::runtime_error("No equal found in parameter");
        }

        std::string key = pair.substr(0, eq);
        std::string val = pair.substr(eq + 1);
        boost::trim(key);
        boost::trim(val);
        
        _props[key] = val;
        
        if (sep == std::string::npos) break;
        else buf = buf.substr(sep+1);

        boost::trim(buf);
      }
    }
  }

  const std::string& content_type::get_prop(const std::string& key,
                                            const std::string& def) const {
    auto it = _props.find(key);
    if (it == _props.end()) return def;
    else return it->second;
  }
  
}
