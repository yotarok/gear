#ifndef gear_data_source_hpp_
#define gear_data_source_hpp_

#include <sndfile.h>


namespace gear {
  template <typename Elem>
  class source {
  };  

  class sndfile_source : public source<float> {
    SF_INFO _info;
    SNDFILE* _sndfile;

    //SF_VIRTUAL_IO _vio;
    //std::istream* pis;
  public:
    /*
    SndFileSource(std::istream& is) {
      pis = &
      _vio.sf_vio_get_filelen = &sndfile_vio_istream_get_filelen;
        &sndfile_vio_istream_seek,
        &sndfile_vio_istream_read,
        &sndfile_vio_istream_write,
        &sndfile_vio_istream_tell,
      };
      _sndfile = ::sf_open_virtual(filepath.c_str(), SFM_READ, &_info);

      INFO("Sndfile: frames=%d, samplerate=%d, channels=%d, format=0x%0x, "
           "sections=%d, seekable=%d",
           _info.frames, _info.samplerate, _info.channels, _info.format, 
           _info.sections, _info.seekable);
      
    }
    */

    sndfile_source(const std::string& filepath) {
      _sndfile = ::sf_open(filepath.c_str(), SFM_READ, &_info);

      INFO("Sndfile: frames=%d, samplerate=%d, channels=%d, format=0x%0x, "
           "sections=%d, seekable=%d",
           _info.frames, _info.samplerate, _info.channels, _info.format, 
           _info.sections, _info.seekable);

    }
    virtual ~sndfile_source() {
      ::sf_close(_sndfile);
    }

  };
}

#endif
