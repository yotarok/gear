#ifndef gear_flow_preset_hpp_
#define gear_flow_preset_hpp_

#include <gear/flow/flow.hpp>

#include <gear/node/node.hpp>
#include <gear/node/window.hpp>
#include <gear/node/fft.hpp>
#include <gear/node/fir.hpp>
#include <gear/node/melfb.hpp>
#include <gear/node/elemfunc.hpp>
#include <gear/node/affinetrans.hpp>
#include <gear/node/htkcompat.hpp>
#include <gear/node/energy.hpp>
#include <gear/node/concat.hpp>
#include <gear/node/delta.hpp>
#include <gear/node/thru.hpp>
#include <gear/node/noise.hpp>
#include <gear/node/meannorm.hpp>

namespace gear {
  flow_ptr eval_preset_name(const std::string& name);

  flow_ptr preset_mfcc_e_d_a(int sample_rate);
  flow_ptr preset_lfbe_e(int sample_rate, int nchans);
  flow_ptr preset_lfbe_e_z(int sample_rate, int nchans);
  flow_ptr preset_lfbe_e_d_a_z(int sample_rate, int nchans);
}

#endif
