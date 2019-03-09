#include <gear/io/matrix.hpp>
namespace gear {

  void write_htk(std::ostream& os, const numeric<float>::matrix& data,
                 uint32_t samp_period, uint16_t parm_kind,
                 bool is_BE) {
    write_scalar<uint32_t>(os, data.cols(), is_BE);
    write_scalar<uint32_t>(os, samp_period, is_BE);
    write_scalar<uint16_t>(os, sizeof(float) * data.rows(), is_BE);
    write_scalar<uint16_t>(os, parm_kind, is_BE);
    for (int j = 0; j < data.cols(); ++ j) {
      for (int i = 0; i < data.rows(); ++ i) {
        write_scalar<float>(os, data(i, j), is_BE);
      }
    }
  }

  void read_htk(numeric<float>::matrix* pdata,
                uint32_t* pperiod, uint16_t* pkind,
                std::istream& is,
                bool is_BE) {
    if (! pdata) throw std::runtime_error("No destination specified");
    
    uint32_t len = read_scalar<uint32_t>(is, is_BE);
    *pperiod = read_scalar<uint32_t>(is, is_BE);
    uint16_t sampwid = read_scalar<uint16_t>(is, is_BE);
    *pkind = read_scalar<uint16_t>(is, is_BE);
    uint16_t dim = sampwid / sizeof(float);
    
    *pdata = numeric<float>::matrix::Zero(dim, len);
    for (int j = 0; j < len; ++ j) {
      for (int i = 0; i < dim; ++ i) {
        (*pdata)(j, i) = read_scalar<float>(is, is_BE);
      }
    }
  }

  void read_fmatrix(numeric<float>::matrix* dest,
                    const std::string& location,
                    const content_type& type) {
    if (type.type() == "application/htk") {
      std::ifstream ifs(location.c_str());
      uint32_t samp_period;
      uint16_t parm_kind;
      read_htk(dest, &samp_period, &parm_kind, ifs,
               type.get_prop("endian", "BE") == "BE");
    } else if (type.type() == "audio/wav" || type.type() == "audio/flac" ||
               type.type() == "audio/x-nist") {
      SF_INFO info;
      read_sndfile(dest, &info, location);
    } else {
      throw std::runtime_error("Unknown type: " + type.type());
    }
  }

  void read_sndfile(numeric<float>::matrix* pdata,
                    ::SF_INFO* pinfo,
                    const std::string& path) {
    if (! pdata) throw std::runtime_error("No destination specified");
      
    SF_INFO info;
    ::SNDFILE* sndf = ::sf_open(path.c_str(), SFM_READ, &info);
    ::sf_command (sndf, SFC_SET_NORM_FLOAT, NULL, SF_FALSE);

    *pdata = numeric<float>::matrix::Zero(info.channels, info.frames);
    ::sf_readf_float(sndf, pdata->data(), info.frames);
    ::sf_close(sndf);

    if (pinfo) *pinfo = info;
  }

}
