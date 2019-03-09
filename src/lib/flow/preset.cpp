#include <gear/flow/preset.hpp>

namespace gear {

  flow_ptr eval_preset_name(const std::string& name) {
    // TO DO: Rewrite with boost::spirit::qi or chaiscript
    flow_ptr ret;
    if (name == "mfcc_e_d_a(8000)") {
      ret = preset_mfcc_e_d_a(8000);
    } else if (name == "mfcc_e_d_a(16000)") {
      ret = preset_mfcc_e_d_a(16000);
    } else if (name == "lfbe_e(16000,39)") {
      ret = preset_lfbe_e(16000, 39);
    } else if (name == "lfbe_e_z(16000,39)") {
      ret = preset_lfbe_e_z(16000, 39);
    } else if (name == "lfbe_e_d_a_z(16000,39)") {
      ret = preset_lfbe_e_d_a_z(16000, 39);
    } else {
      throw std::runtime_error("unknown preset name");
    }
    return ret;

    
  }

  flow_ptr preset_mfcc_e_d_a(int sample_rate) {
    flow_ptr ret(new flow);

    auto addnoise = new Gaussian_noise_node("noise", "gauss_noise_node");
    auto win = new window_node<float>("win", "window_node");
    auto preemph = new HTK_preemph_node("preemph", "preemph_node");
    auto en = new energy_node("en", "energy_node");
    auto fft = new FFT_node("fft", "FFT_node");
    auto melfb = new MelFB_node("melfb", "MelFB");
    auto lfbe = new elemwise_func_node("lfbe", "elemwise_func_node");
    auto le = new elemwise_func_node("le", "elemwise_func_node");
    auto dct = new DCT_node("dct", "DCT_node");
    auto lift = new HTK_cepstral_scaler_node("lift", "HTK_cepstral_scaler_node");
    auto concat_MFCC_E = new concat_node<float> ("concat_MFCC_E", "concat_node");
    auto concat_D_A = new concat_node<float>("concat_D_A", "concat_node");
    auto delta_D = new delta_node("delta_D", "delta_node");
    auto delta_A = new delta_node("delta_A", "delta_node");

    auto output = new thru_node("_output", "thru_node");
    auto input = new thru_node("_input", "thru_node");

    addnoise->set_stdev(1.0);
    
    win->set_size(sample_rate * 0.025);
    win->set_shift(sample_rate * 0.010);
    preemph->set_coef(0.97);
    fft->set_window(FFT_node::window_hamming);

    int nfftbin = 2;
    while (nfftbin < sample_rate * 0.025) nfftbin *= 2;

    fft->set_nbins(nfftbin);
    melfb->set_nchans(26);
    melfb->set_nyquist_hz(sample_rate / 2);
    lfbe->set_func_type(elemwise_func_node::func_log);
    le->set_func_type(elemwise_func_node::func_log);
    dct->set_offset(1);
    dct->set_ncoefs(12);
    lift->set_offset(1);
    lift->set_const(22);
    concat_MFCC_E->set_nports(2);
    concat_D_A->set_nports(3);
    delta_D->set_halfwidth(2);
    delta_A->set_halfwidth(2);
   
    ret->add(addnoise);
    ret->add(win);
    ret->add(preemph);
    ret->add(en);
    ret->add(fft);
    ret->add(melfb);
    ret->add(lfbe);
    ret->add(le);
    ret->add(dct);
    ret->add(lift);
    ret->add(concat_MFCC_E);
    ret->add(concat_D_A);
    ret->add(delta_D);
    ret->add(delta_A);
    ret->add(input);
    ret->add(output);

    ret->connect(input->get_id(), "", addnoise->get_id(), "");
    ret->connect(addnoise->get_id(), "", win->get_id(), "");
    ret->connect(win->get_id(), "", preemph->get_id(), "");
    ret->connect(preemph->get_id(), "", fft->get_id(), "");
    ret->connect(fft->get_id(), "", melfb->get_id(), "");
    ret->connect(melfb->get_id(), "", lfbe->get_id(), "");
    ret->connect(lfbe->get_id(), "", dct->get_id(), "");
    ret->connect(dct->get_id(), "", lift->get_id(), "");
    ret->connect(lift->get_id(), "", concat_MFCC_E->get_id(), "0");

    ret->connect(win->get_id(), "", en->get_id(), "");
    ret->connect(en->get_id(), "", le->get_id(), "");
    ret->connect(le->get_id(), "", concat_MFCC_E->get_id(), "1");

    ret->connect(concat_MFCC_E->get_id(), "", concat_D_A->get_id(), "0");
    ret->connect(concat_MFCC_E->get_id(), "", delta_D->get_id(), "");
    ret->connect(delta_D->get_id(), "", concat_D_A->get_id(), "1");
    ret->connect(delta_D->get_id(), "", delta_A->get_id(), "");
    ret->connect(delta_A->get_id(), "", concat_D_A->get_id(), "2");
    
    ret->connect(concat_D_A->get_id(), "", output->get_id(), "");
    
    
    return ret;
  }

  flow_ptr preset_lfbe_e(int sample_rate, int nchans) {
    flow_ptr ret(new flow);

    auto addnoise = new Gaussian_noise_node("noise", "gauss_noise_node");
    auto win = new window_node<float>("win", "window_node");
    auto preemph = new HTK_preemph_node("preemph", "preemph_node");
    auto en = new energy_node("en", "energy_node");
    auto fft = new FFT_node("fft", "FFT_node");
    auto melfb = new MelFB_node("melfb", "MelFB");
    auto lfbe = new elemwise_func_node("lfbe", "elemwise_func_node");
    auto le = new elemwise_func_node("le", "elemwise_func_node");
    auto concat_LFBE_E = new concat_node<float> ("concat_LFBE_E", "concat_node");

    auto output = new thru_node("_output", "thru_node");
    auto input = new thru_node("_input", "thru_node");

    addnoise->set_stdev(1.0);
    
    win->set_size(sample_rate * 0.025);
    win->set_shift(sample_rate * 0.010);
    preemph->set_coef(0.97);
    fft->set_window(FFT_node::window_hamming);

    int nfftbin = 2;
    while (nfftbin < sample_rate * 0.025) nfftbin *= 2;

    fft->set_nbins(nfftbin);
    melfb->set_nchans(nchans);
    melfb->set_nyquist_hz(sample_rate / 2);
    lfbe->set_func_type(elemwise_func_node::func_log);
    le->set_func_type(elemwise_func_node::func_log);
    concat_LFBE_E->set_nports(2);
   
    ret->add(addnoise);
    ret->add(win);
    ret->add(preemph);
    ret->add(en);
    ret->add(fft);
    ret->add(melfb);
    ret->add(lfbe);
    ret->add(le);
    ret->add(concat_LFBE_E);
    ret->add(input);
    ret->add(output);

    ret->connect(input->get_id(), "", addnoise->get_id(), "");
    ret->connect(addnoise->get_id(), "", win->get_id(), "");
    ret->connect(win->get_id(), "", preemph->get_id(), "");
    ret->connect(preemph->get_id(), "", fft->get_id(), "");
    ret->connect(fft->get_id(), "", melfb->get_id(), "");
    ret->connect(melfb->get_id(), "", lfbe->get_id(), "");
    ret->connect(lfbe->get_id(), "", concat_LFBE_E->get_id(), "0");

    ret->connect(win->get_id(), "", en->get_id(), "");
    ret->connect(en->get_id(), "", le->get_id(), "");
    ret->connect(le->get_id(), "", concat_LFBE_E->get_id(), "1");
   
    ret->connect(concat_LFBE_E->get_id(), "", output->get_id(), "");    
    
    return ret;
  }

  flow_ptr preset_lfbe_e_z(int sample_rate, int nchans) {
    // lfbe_e + Utterance-based mean normalization
    flow_ptr ret(new flow);

    auto addnoise = new Gaussian_noise_node("noise", "gauss_noise_node");
    auto win = new window_node<float>("win", "window_node");
    auto preemph = new HTK_preemph_node("preemph", "preemph_node");
    auto en = new energy_node("en", "energy_node");
    auto fft = new FFT_node("fft", "FFT_node");
    auto melfb = new MelFB_node("melfb", "MelFB");
    auto lfbe = new elemwise_func_node("lfbe", "elemwise_func_node");
    auto le = new elemwise_func_node("le", "elemwise_func_node");
    auto concat_LFBE_E = new concat_node<float> ("concat_LFBE_E", "concat_node");
    auto meannorm = new batch_meannorm_node("mean_norm", "batch_meannorm_node");

    auto output = new thru_node("_output", "thru_node");
    auto input = new thru_node("_input", "thru_node");

    addnoise->set_stdev(1.0);
    
    win->set_size(sample_rate * 0.025);
    win->set_shift(sample_rate * 0.010);
    preemph->set_coef(0.97);
    fft->set_window(FFT_node::window_hamming);

    int nfftbin = 2;
    while (nfftbin < sample_rate * 0.025) nfftbin *= 2;

    fft->set_nbins(nfftbin);
    melfb->set_nchans(nchans);
    melfb->set_nyquist_hz(sample_rate / 2);
    lfbe->set_func_type(elemwise_func_node::func_log);
    le->set_func_type(elemwise_func_node::func_log);
    concat_LFBE_E->set_nports(2);
   
    ret->add(addnoise);
    ret->add(win);
    ret->add(preemph);
    ret->add(en);
    ret->add(fft);
    ret->add(melfb);
    ret->add(lfbe);
    ret->add(le);
    ret->add(concat_LFBE_E);
    ret->add(meannorm);
    ret->add(input);
    ret->add(output);

    ret->connect(input->get_id(), "", addnoise->get_id(), "");
    ret->connect(addnoise->get_id(), "", win->get_id(), "");
    ret->connect(win->get_id(), "", preemph->get_id(), "");
    ret->connect(preemph->get_id(), "", fft->get_id(), "");
    ret->connect(fft->get_id(), "", melfb->get_id(), "");
    ret->connect(melfb->get_id(), "", lfbe->get_id(), "");
    ret->connect(lfbe->get_id(), "", concat_LFBE_E->get_id(), "0");

    ret->connect(win->get_id(), "", en->get_id(), "");
    ret->connect(en->get_id(), "", le->get_id(), "");
    ret->connect(le->get_id(), "", concat_LFBE_E->get_id(), "1");
   
    ret->connect(concat_LFBE_E->get_id(), "", meannorm->get_id(), "");    
    ret->connect(meannorm->get_id(), "", output->get_id(), "");    
    
    return ret;
    
  }

  flow_ptr preset_lfbe_e_d_a_z(int sample_rate, int nchans) {
    // lfbe_e + Utterance-based mean normalization
    flow_ptr ret(new flow);

    auto addnoise = new Gaussian_noise_node("noise", "gauss_noise_node");
    auto win = new window_node<float>("win", "window_node");
    auto preemph = new HTK_preemph_node("preemph", "preemph_node");
    auto en = new energy_node("en", "energy_node");
    auto fft = new FFT_node("fft", "FFT_node");
    auto melfb = new MelFB_node("melfb", "MelFB");
    auto lfbe = new elemwise_func_node("lfbe", "elemwise_func_node");
    auto le = new elemwise_func_node("le", "elemwise_func_node");
    auto concat_LFBE_E = new concat_node<float> ("concat_LFBE_E", "concat_node");
    auto delta_D = new delta_node("delta_D", "delta_node");
    auto delta_A = new delta_node("delta_A", "delta_node");
    auto concat_D_A = new concat_node<float>("concat_D_A", "concat_node");

    auto meannorm = new batch_meannorm_node("mean_norm", "batch_meannorm_node");

    auto output = new thru_node("_output", "thru_node");
    auto input = new thru_node("_input", "thru_node");

    addnoise->set_stdev(1.0);
    
    win->set_size(sample_rate * 0.025);
    win->set_shift(sample_rate * 0.010);
    preemph->set_coef(0.97);
    fft->set_window(FFT_node::window_hamming);

    int nfftbin = 2;
    while (nfftbin < sample_rate * 0.025) nfftbin *= 2;

    fft->set_nbins(nfftbin);
    melfb->set_nchans(nchans);
    melfb->set_nyquist_hz(sample_rate / 2);
    lfbe->set_func_type(elemwise_func_node::func_log);
    le->set_func_type(elemwise_func_node::func_log);
    concat_LFBE_E->set_nports(2);
    concat_D_A->set_nports(3);
    delta_D->set_halfwidth(2);
    delta_A->set_halfwidth(2);
   
    ret->add(addnoise);
    ret->add(win);
    ret->add(preemph);
    ret->add(en);
    ret->add(fft);
    ret->add(melfb);
    ret->add(lfbe);
    ret->add(le);
    ret->add(concat_LFBE_E);
    ret->add(delta_D);
    ret->add(delta_A);
    ret->add(concat_D_A);
    ret->add(meannorm);
    ret->add(input);
    ret->add(output);

    ret->connect(input->get_id(), "", addnoise->get_id(), "");
    ret->connect(addnoise->get_id(), "", win->get_id(), "");
    ret->connect(win->get_id(), "", preemph->get_id(), "");
    ret->connect(preemph->get_id(), "", fft->get_id(), "");
    ret->connect(fft->get_id(), "", melfb->get_id(), "");
    ret->connect(melfb->get_id(), "", lfbe->get_id(), "");
    ret->connect(lfbe->get_id(), "", concat_LFBE_E->get_id(), "0");

    ret->connect(win->get_id(), "", en->get_id(), "");
    ret->connect(en->get_id(), "", le->get_id(), "");
    ret->connect(le->get_id(), "", concat_LFBE_E->get_id(), "1");
   
    ret->connect(concat_LFBE_E->get_id(), "", delta_D->get_id(), "");
    ret->connect(concat_LFBE_E->get_id(), "", concat_D_A->get_id(), "0");
    ret->connect(delta_D->get_id(), "", delta_A->get_id(), "");
    ret->connect(delta_D->get_id(), "", concat_D_A->get_id(), "1");
    ret->connect(delta_A->get_id(), "", concat_D_A->get_id(), "2");

    ret->connect(concat_D_A->get_id(), "", meannorm->get_id(), "");    
    ret->connect(meannorm->get_id(), "", output->get_id(), "");    
    
    return ret;
    
  }


}

