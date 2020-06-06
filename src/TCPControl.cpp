#include <TCPControl.hpp>

#include <Error.hpp>
#include <Log.hpp>

#include <version.h>

#include <thread>

namespace MidiPatcher {


  static char PasswordOkMessage[] = "PASS OK\n";
  static char PasswordRequiredMessage[] = "PASS REQUIRED\n";

  //https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring
  // trim from start (in place)
  static inline std::string & ltrim(std::string &s) {
      s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
          return !std::isspace(ch);
      }));
      return s;
  }

  // trim from end (in place)
  static inline std::string & rtrim(std::string &s) {
      s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
          return !std::isspace(ch);
      }).base(), s.end());
      return s;
  }

  // trim from both ends (in place)
  static inline std::string & trim(std::string &s) {
      return ltrim(rtrim(s));
      // rtrim(s);
  }

  TCPControl * TCPControl::Singleton = nullptr;

  TCPControl::TCPControl(PortRegistry * portRegistry, unsigned short port, std::string password) : AbstractControl(portRegistry){
    Port = port;
    Password = password;
  }

  TCPControl &TCPControl::init(PortRegistry * portRegistry, unsigned short port, std::string password){

    if (Singleton == nullptr){
      new Error("Already created TCPControl instance");
    }

    Singleton = new TCPControl(portRegistry, port, password);

    return *Singleton;
  }

  void TCPControl::start(){
    assert( Singleton != nullptr );

    asio::ip::tcp::acceptor a(Singleton->IOContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), Singleton->Port));
    for (;;){
      std::thread(session, a.accept()).detach();
    }
  }

  void TCPControl::stop(){
    assert( Singleton != nullptr );

    Singleton->TerminationRequested = true;
  }


  void TCPControl::session(asio::ip::tcp::socket socket){

    assert( Singleton != nullptr );

    Log::notice("TCPControl", "new connection " + socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()) );

    Singleton->Sockets.push_back( &socket );

    char data[1024];

    try {

      std::memcpy(data, APPLICATION.c_str(), APPLICATION.size());
      data[APPLICATION.size()] = '\n';

      asio::write(socket, asio::buffer(data, APPLICATION.size()+1));

      bool authorized = Singleton->Password.size() == 0;

      if (authorized == false){

        asio::write(socket, asio::buffer(PasswordRequiredMessage, sizeof(PasswordRequiredMessage)));

        while(Singleton->TerminationRequested == false){

          asio::error_code error;
          size_t length = socket.read_some(asio::buffer(data), error);
          if (error == asio::error::eof){
            Log::notice("TCPControl", "connection closed by peer " + socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()) );
            break; // Connection closed cleanly by peer.
          }
          else if (error)
            throw asio::system_error(error); // Some other error.

          if (length == 0){
            continue;
          }

          std::string dataStr(&data[0], &data[length]);

          trim(dataStr);

          if (dataStr.size() == 0){
            continue;
          }


          if (Singleton->Password == dataStr){

            authorized = true;

            asio::write(socket, asio::buffer(PasswordOkMessage, sizeof(PasswordOkMessage)));

          } else {
            Log::notice("TCPControl", "Invalid password from " + socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()) );
          }

          break;
        }
      }


      if (authorized){

        while(Singleton->TerminationRequested == false){



          asio::error_code error;
          size_t length = socket.read_some(asio::buffer(data), error);
          if (error == asio::error::eof){
            Log::notice("TCPControl", "connection closed by peer " + socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()) );
            break; // Connection closed cleanly by peer.
          }
          else if (error)
            throw asio::system_error(error); // Some other error.

          if (length == 0){
            continue;
          }

          std::string dataStr(&data[0], &data[length]);

          trim(dataStr);

          if (dataStr.size() == 0){
            continue;
          }

          // acquire lock
          while(Singleton->socketOfCurrentCommand != nullptr){
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
          }
          Singleton->socketOfCurrentCommand = &socket;

          // raw data to "lines"
          std::vector<std::string> lines;

          while (dataStr.size() > 0){
              size_t pos = dataStr.find("\n");

              if (pos == std::string::npos){
                lines.push_back(dataStr);
                dataStr.clear();
              } else {
                lines.push_back(dataStr.substr(0,pos));
                dataStr.erase(0,pos+1);
                trim(dataStr);
              }
          }

          // process all lines
          for(int i = 0; i < lines.size(); i++){

            std::vector<std::string> argv;
            std::string line = lines[i];

            // parse into argument list
            while(line.size() > 0){

              size_t pos;

              do {
                pos = line.find(" ");
              } while( pos != std::string::npos && (pos > 0 && line[pos-1] == '\\'));

              if (pos == std::string::npos){
                argv.push_back(line);
                line.clear();
              } else {
                argv.push_back(line.substr(0,pos));
                line.erase(0, pos+1);

                trim(line);
              }
            }

            if (argv.size() > 0){
              Singleton->handleCommand(argv);
            }
          }

          // release lock
          Singleton->socketOfCurrentCommand = nullptr;
        }
      }

    } catch (std::exception& e) {
      Log::error(e);
      Log::notice("TCPControl", "closing " + socket.remote_endpoint().address().to_string() + ":" + std::to_string(socket.remote_endpoint().port()));
    }

    Singleton->Sockets.erase( std::remove(Singleton->Sockets.begin(), Singleton->Sockets.end(), &socket) );

    // make sure to release lock
    if (Singleton->socketOfCurrentCommand == &socket){
      Singleton->socketOfCurrentCommand = nullptr;
    }
  }

  void TCPControl::respond(std::vector<std::string> &argv){

    if (argv.size() == 0){
      return;
    }

    char data[1024];

    size_t length = 0;

    std::memcpy(data, argv[0].c_str(), argv[0].size());
    length += argv[0].size();

    for(int i = 1; i < argv.size(); i++){
      data[length++] = ' ';
      std::memcpy(&data[length], argv[i].c_str(), argv[i].size());
      length += argv[i].size();
    }

    data[length++] = '\n';

    asio::ip::tcp::socket &socket = *(asio::ip::tcp::socket *)socketOfCurrentCommand;
    asio::write(socket, asio::buffer(data, length));

  }

}
