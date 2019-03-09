#include <gear/io/logging.hpp>
#include <gear/data/source.hpp>
#include <gear/tool/args.hpp>
#include <gear/tool/main.hpp>
#include <gear/io/matrix.hpp>

#include <tclap/CmdLine.h>
#include <iostream>
#include <streambuf>
#include <fstream>

#include <gear/flow/preset.hpp>
#include <gear/utils.hpp>

namespace gear {
  DEFINE_ARGCLASS(arg_type, (common_args),
                  (TCLAP::ValueArg<std::string>, flow, 
                   ("f", "flow", "rspec", false, "", "FILE")),
                  (TCLAP::ValueArg<std::string>, preset, 
                   ("p", "preset", "preset name", false, "", "NAME")),
                  (TCLAP::MultiArg<std::string>, input_descr,
                   ("i", "input", "input descriptor", false, "FILE")),
                  (TCLAP::MultiArg<std::string>, output_descr,
                   ("o", "output", "output descriptor", false, "FILE"))
                  );

  // The following read and write function should be merged to
  // read_fmatrix function in matrix.hpp
  // But, so far, the following is used since it requires further effort
  // for command line parsing of IODescr with MIME content-type
  void read_matrix(const std::string& path, const std::string& filetype,
                   numeric<float>::matrix& data,
                   int* prate) {
    if (filetype == "wav") {
      SF_INFO info;
      read_sndfile(&data, &info, path);
      if (prate) *prate = info.samplerate;
    } else if (filetype == "htk") {
      std::ifstream ifs(path.c_str());
      uint32_t sampp;
      uint16_t kind;
      read_htk(&data, &sampp, &kind, ifs);
      if (prate) *prate = static_cast<int>(10000000.0 / sampp);
    } else {
      throw std::runtime_error("Unknown type: " + filetype);
    }
  }

  void write_matrix(const std::string& path, const std::string& filetype,
                    const numeric<float>::matrix& data) {
    if (filetype == "csv") {
      std::ofstream ofs(path.c_str());
      write_csv<float>(ofs, data);
    } else {
      throw std::runtime_error("Unknown type: " + filetype);
    }
  }

  struct io_descr {
    std::string path, filetype, nodeid, portid;
  };

  void parse_iodescr(std::string s, io_descr& descr, 
                     const std::string& default_nodeport,
                     const std::string& default_type) {
    std::string nodeportid = default_nodeport;
    INFO("parsing %s", s.c_str());

    size_t pos = s.find('@');
    if (pos != std::string::npos) {
      nodeportid = s.substr(pos + 1);
      s = s.substr(0, pos);
      INFO("nodeportid = %s, %d", nodeportid.c_str(), pos);
    }

    pos = nodeportid.find('[');
    if (pos != std::string::npos) {
      descr.nodeid = nodeportid.substr(0, pos);
      descr.portid = nodeportid.substr(pos + 1, nodeportid.size() - pos - 2);
    } else {
      descr.nodeid = nodeportid;
      descr.portid = "";
    }

    pos = s.find(':');
    if (pos != std::string::npos) {
      descr.path = s.substr(0, pos);
      descr.filetype = s.substr(pos + 1);
    } else {
      descr.path = s;
      // TODO: add type inference
      descr.filetype = default_type;
    }    

  }

  int tool_main(arg_type& arg, int argc, char* argv[]) {

    // multiple I/O
    // gear_feed --preset mfcc_e_d_a -i hoge.wav -o hoge.mfcc_e_d_a:htk -o hoge.lfbe.csv:csv@lfbe[portname]

    flow_ptr flow;
    if (arg.flow.isSet()) {
      flow = parse_flow(arg.flow.getValue());
    } else if (arg.preset.isSet()) {
      flow = eval_preset_name(arg.preset.getValue());
    } else {
      throw std::runtime_error("Set flow yaml or preset name");
    }

    std::vector<flow_node_ptr> outputs;
    for (int n = 0; n < arg.output_descr.getValue().size(); ++ n) {
      io_descr descr;
      parse_iodescr(arg.output_descr.getValue()[n], descr,
                    "_output", "csv");

      auto sink = 
        new vector_sink_node<float>((boost::format("sink_%1%") % n).str(),
                                    "vector_sink_node");
      
      flow->node(descr.nodeid)
        ->connect(descr.portid, sink->input_port(""));      
      outputs.push_back(flow->add(sink));
    }

    std::vector<flow_node_ptr> inputs;
    
    for (int n = 0; n < arg.input_descr.getValue().size(); ++ n) {
      io_descr descr;
      parse_iodescr(arg.input_descr.getValue()[n], descr,
                    "_input", "wav");

      numeric<float>::matrix data;
      int rate;
      INFO("Read from %s [type=%d]", descr.path.c_str(), descr.filetype.c_str());
      read_matrix(descr.path, descr.filetype, data, &rate);
      auto src = 
        new vector_source_node<float>((boost::format("source_%1%") % n).str(),
                                      "vector_source_node");

      src->connect("", flow->node(descr.nodeid)->input_port(descr.portid));

      src->set_meta_data(data.rows(), rate, descr.path);
      src->append_data(data);

      inputs.push_back(flow->add(src));
    }

    feed_forward(flow, inputs);
    
    for (int n = 0; n < arg.output_descr.getValue().size(); ++ n) {
      io_descr descr;
      parse_iodescr(arg.output_descr.getValue()[n], descr,
                    "_output", "csv");

      auto p = std::dynamic_pointer_cast<vector_sink_node<float> >(outputs[n]);
      INFO("Write to %s [type=%s, nframes=%d]",
           descr.path.c_str(), descr.filetype.c_str(), p->get_output().cols());
      write_matrix(descr.path, descr.filetype, p->get_output());
    }

    return 0;
  }
}

int main(int argc, char* argv[]) {
  return gear::wrap_main("Feed signals to a flow and write output",
                          argc, argv, gear::tool_main);
}
