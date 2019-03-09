#ifndef gear_data_matrix_hpp_
#define gear_data_matrix_hpp_

#include <gear/types.hpp>
#include <gear/io/binary.hpp>
#include <gear/io/fileformat.hpp>

#include <sndfile.h>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <boost/optional.hpp>

#define HTK_PARMKIND_USER 9
#define HTK_PARMKIND_DEFAULT HTK_PARMKIND_USER

namespace gear {
  void write_htk(std::ostream& os, const numeric<float>::matrix& data,
                 uint32_t samp_period, uint16_t parm_kind = HTK_PARMKIND_DEFAULT,
                 bool is_BE = true);
  
  void read_htk(numeric<float>::matrix* pdata,
                uint32_t* pperiod, uint16_t* pkind,
                std::istream& is,
                bool is_BE = true);
  
  void read_sndfile(numeric<float>::matrix* pdata,
                    ::SF_INFO* pinfo,
                    const std::string& path);
  
  template <typename NumT>
  void write_csv(std::ostream& os, const typename numeric<NumT>::matrix& data) {
    // In CSV, for line-based operations, transposed data are stored.
    for (int t = 0; t < data.cols(); ++ t) { 
      os << data(0, t);
      for (int d = 1; d < data.rows(); ++ d) {
        os << ',' << data(d, t);
      }
      os << std::endl; 
      // ^ this might be a performance bottleneck, replace endl to '\n' 
      // if this become a big issue
    }
  }

  
  void read_fmatrix(numeric<float>::matrix* dest,
                    const std::string& location,
                    const content_type& type);
  

  // TO DO: Unify with spin::convert_to_matrix
  template <typename NumT>
  void
  read_yaml_matrix(Eigen::Matrix<NumT, Eigen::Dynamic, Eigen::Dynamic>* dest,
                   const YAML::Node& node) {
    bool transpose;
    transpose = node["transpose"] && node["transpose"].as<bool>();
    YAML::Node data = node["data"];
    bool inited = false;
    for (int row = 0; row < data.size(); ++ row) {
      YAML::Node rowdata = data[row];
      if (rowdata.IsScalar()) {
        if (! inited)
          dest->resize(data.size(), 1);
        (*dest)(row, 0) = rowdata.as<NumT>();
      } else {
        for (int col = 0; col < rowdata.size(); ++ col) {
          if (! inited) {
            std::cout <<  "inited" << std::endl;
            if (transpose) {
              dest->resize(rowdata.size(), data.size());
            } else {
              dest->resize(data.size(), rowdata.size());
            }
            inited = true;
          }
          
          if (transpose) {
            (*dest)(col, row) = rowdata[col].as<NumT>();
          } else {
            (*dest)(row, col) = rowdata[col].as<NumT>();
          }
        }
      }
    }
  }
  
}

#endif
