#ifndef gear_io_fileformat_hpp_
#define gear_io_fileformat_hpp_

#include <string>
#include <map>

namespace gear {

  class content_type {
    std::string _type;
    std::map<std::string, std::string> _props;
    
  public:
    content_type(const std::string& s);
    void parse_and_assign(const std::string& s);

    const std::string& type() const { return _type; }
    const std::string& get_prop(const std::string& key,
                                const std::string& def) const;
  };

}

#endif
