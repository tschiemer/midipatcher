#include <Port/Serial.hpp>

#include <Error.hpp>

namespace MidiPatcher {
  namespace Port {

    AbstractPort* Serial::factory(PortDescriptor &portDescriptor){
      assert( portDescriptor.PortClass == PortClass );

      std::string device;
      if (portDescriptor.Options.count("device") == 0){
        device = portDescriptor.Name;
      } else {
        device = portDescriptor.Options["device"];
      }

      Serial * serial = new Serial(portDescriptor.Name, device);

      serial->setOptions(portDescriptor.Options);

      return serial;
    }

    std::vector<AbstractPort*>  * Serial::scanner(){
      return new std::vector<AbstractPort*>();
    }

    Serial::Serial(std::string portName, std::string device) : AbstractPort(TypeInputOutput, portName), SerialPort(IOService, device){
      Device = device;
    }

    Serial::~Serial(){

    }


    bool Serial::hasOption(std::string key){
      if (key == "runningstatus") return true;
      if (key == "baudrate") return true;
      if (key == "flowcontrol") return true;
      if (key == "databits") return true;
      if (key == "stopbits") return true;
      if (key == "parity") return true;

      return false;
    }

    std::string Serial::getOption(std::string key){
      if (key == "runningstatus") {
        return (getInRunningStatusEnabled() ? "1" : "0");
      }
      if (key == "baudrate") {
        asio::serial_port_base::baud_rate br;

        SerialPort.get_option(br);

        return std::to_string(br.value());
      }
      if (key == "flowcontrol") {
        asio::serial_port_base::flow_control fc;

        SerialPort.get_option(fc);

        if (fc.value() == asio::serial_port_base::flow_control::none) return "none";
        if (fc.value() == asio::serial_port_base::flow_control::software) return "software";
        if (fc.value() == asio::serial_port_base::flow_control::hardware) return "hardware";

        throw Error("Unrecognized asio flowcontrol option " + std::to_string(fc.value()));
      }
      if (key == "databits") {
        asio::serial_port_base::character_size db;

        SerialPort.get_option(db);

        return std::to_string(db.value());
      }
      if (key == "stopbits") {
        asio::serial_port_base::stop_bits sb;

        SerialPort.get_option(sb);

        if (sb.value() == asio::serial_port_base::stop_bits::one) return "1";
        if (sb.value() == asio::serial_port_base::stop_bits::onepointfive) return "1.5";
        if (sb.value() == asio::serial_port_base::stop_bits::two) return "2";

        throw Error("Unrecognized asio stopbits option " + std::to_string(sb.value()));
      }
      if (key == "parity") {
        asio::serial_port_base::parity p;

        SerialPort.get_option(p);

        if (p.value() == asio::serial_port_base::parity::none) return "none";
        if (p.value() == asio::serial_port_base::parity::odd) return "odd";
        if (p.value() == asio::serial_port_base::parity::even) return "even";

        throw Error("Unrecognized asio parity option " + std::to_string(p.value()));
      }

      throw Error("Unrecognized option " + key);
    }

    void Serial::setOption(std::string key, std::string value){
        if (key == "runningstatus") {
          bool enabled = (bool)std::stoi(value);
          setInRunningStatusEnabled(enabled);
          setOutRunningStatusEnabled(enabled);
        }
        if (key == "baudrate"){
          int br = std::stoi(value);
          SerialPort.set_option(asio::serial_port_base::baud_rate(br));
        }
        if (key == "flowcontrol"){
          if (value == "none"){
            SerialPort.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::none));
          } else if (value == "software"){
            SerialPort.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::software));
          } else if (value == "hardware"){
            SerialPort.set_option(asio::serial_port_base::flow_control(asio::serial_port_base::flow_control::hardware));
          } else {
            throw Error("Invalid value " + value + " for option " + key);
          }
        }
        if (key == "databits"){
          int db = std::stoi(value);
          if (5 <= db && db <= 8){
            SerialPort.set_option(asio::serial_port_base::character_size(db));
          } else {
            throw Error("Invalid value " + value + " for option " + key);
          }
        }
        if (key == "stopbits"){
          if (value == "1"){
            SerialPort.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::one));
          } else if (value == "1.5"){
            SerialPort.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::onepointfive));
          } else if (value == "2"){
            SerialPort.set_option(asio::serial_port_base::stop_bits(asio::serial_port_base::stop_bits::two));
          } else {
            throw Error("Invalid value " + value + " for option " + key);
          }
        }
        if (key == "parity"){
          if (value == "none"){
            SerialPort.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::none));
          } else if (value == "odd"){
            SerialPort.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::odd));
          } else if (value == "even"){
            SerialPort.set_option(asio::serial_port_base::parity(asio::serial_port_base::parity::even));
          } else {
            throw Error("Invalid value " + value + " for option " + key);
          }
        }


        throw Error("Unrecognized option " + key);
    }

    void Serial::start(){
      if (SerialPort.is_open()){
        return;
      }
      SerialPort.open(Device);
    }

    void Serial::stop(){
      if (SerialPort.is_open() == false){
        return;
      }
      SerialPort.close();
    }

    void Serial::writeToStream(unsigned char * data, size_t len){
      if (SerialPort.is_open() == false){
        return;
      }

      SerialPort.write_some(asio::buffer(data,len));
    }
  }
}
