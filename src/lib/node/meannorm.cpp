#include <gear/node/meannorm.hpp>

#include <gear/node/factory.hpp>
#include <gear/io/matrix.hpp>

namespace gear {
  flow_node_ptr create_batch_meannorm_node(const YAML::Node& src) {
    batch_meannorm_node* ret =
      new batch_meannorm_node(src["name"].as<std::string>(),
                              src["type"].as<std::string>());
    return flow_node_ptr(ret);
  }

  static node_factory_registry
  batch_meannorm_node_registry("batch_meannorm", create_batch_meannorm_node);

  batch_meannorm_node::batch_meannorm_node(const std::string& id,
                                           const std::string& type)
    : flow_node(id, type), _is_first(false) {
  }

  void batch_meannorm_node::on_BOS(const BOS_packet& packet) {
    _is_first = true;
    _output(packet);
  }

  void batch_meannorm_node::on_EOS(const EOS_packet& packet) {
    numeric<float>::matrix data = _buffer.all();
    data.colwise() -= data.rowwise().sum() / data.cols();
    _output(vector_data_packet<float>(data));
    _output(packet);
  }


  void batch_meannorm_node::on_data(const vector_data_packet<float>& packet) {
    if (packet.data.cols() == 0) return;

    int T = packet.data.cols();
    if (_is_first) {
      _is_first = false;
      _buffer = vector_deque<float>(packet.data.rows(), T * 2);
    }
    _buffer.push_back(packet.data);
  }

  packet_handler_ptr batch_meannorm_node::input_port(const std::string& portname) {
    if (portname.size() != 0) throw port_not_defined(*this, portname);
      
    return 
      packet_handler_ptr
      ((new packet_handler_chain)
       ->add<vector_data_packet<float> >(this, &batch_meannorm_node::on_data)
       ->add<BOS_packet>(this, &batch_meannorm_node::on_BOS)
       ->add<EOS_packet>(this, &batch_meannorm_node::on_EOS)
       ->add_thru<control_packet>(&this->_output)
       );
  }

}
