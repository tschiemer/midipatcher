#ifndef MIDIPATCHER_TCP_CONTROL_H
#define MIDIPATCHER_TCP_CONTROL_H

#include "AbstractControl.hpp"

#include "asio.hpp"

namespace MidiPatcher {

  class TCPControl : public virtual AbstractControl {

    protected:

      static TCPControl * Singleton;

      std::vector<asio::ip::tcp::socket *> Sockets;

      volatile bool TerminationRequested = false;

      unsigned short Port;
      std::string Password;

      asio::io_context IOContext;
      volatile asio::ip::tcp::socket * socketOfCurrentCommand = nullptr;

      TCPControl(PortRegistry * portRegistry, unsigned short port, std::string password = "");

    public:

      static TCPControl &init(PortRegistry * portRegistry, unsigned short port, std::string password = "");

      static void start();
      static void stop();

    protected:

      static void session(asio::ip::tcp::socket socket);

      void respond(std::vector<std::string> &argv);

      void publishNotification(std::vector<std::string> &argv);
  };

}

#endif /* MIDIPATCHER_TCP_CONTROL_H */
