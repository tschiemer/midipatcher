#ifndef MIDIPATCHER_TCP_CONTROL_H
#define MIDIPATCHER_TCP_CONTROL_H

#include "AbstractControl.hpp"

#include "asio.hpp"

namespace MidiPatcher {

  class TCPControl : public virtual AbstractControl {

    protected:

      static TCPControl * Singleton;

      std::vector<asio::ip::tcp::socket *> Sockets;
      std::map<asio::ip::tcp::socket *,std::string> SessionDelimiters;

      volatile bool TerminationRequested = false;

      unsigned short Port;
      std::string Password;
      std::string DefaultDelimiter;

      asio::io_context IOContext;
      volatile asio::ip::tcp::socket * socketOfCurrentCommand = nullptr;

      TCPControl(PortRegistry * portRegistry, unsigned short port, std::string password, std::string defaultDelimiter);

    public:

      static TCPControl &init(PortRegistry * portRegistry, unsigned short port, std::string password = "", std::string defaultDelimiter = " ");

      inline void setPassword(std::string password){
        Password = password;
      }

      inline std::string getDefaultArgvDelimiter(){
        return DefaultDelimiter;
      }

      inline void setDefaultArgvDelimiter(std::string delimiter){
        assert( delimiter.size() > 0 );
        DefaultDelimiter = delimiter;
      }


      inline std::string getSessionArgvDelimiter(asio::ip::tcp::socket * socket){
        if (SessionDelimiters.count(socket) > 0){
          return SessionDelimiters[socket];
        }
        return DefaultDelimiter;
      }

      inline void setSessionArgvDelimiter(asio::ip::tcp::socket * socket, std::string delimiter){
        SessionDelimiters[socket] = delimiter;
      }

      static void start();
      static void stop();

    protected:

      static void session(asio::ip::tcp::socket socket);

      void respond(std::vector<std::string> &argv);

      void publishNotification(std::vector<std::string> &argv);
  };

}

#endif /* MIDIPATCHER_TCP_CONTROL_H */
