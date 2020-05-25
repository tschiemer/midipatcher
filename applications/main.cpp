#include "main.hpp"

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <csignal>
#include <cstdlib>

#include <map>

/***************************/

MidiPatcher::PortRegistry * portRegistry = NULL;
// int sigintTicks = 0;
volatile bool Running = false;

struct {
    int Verbosity;
    bool ShowUpdates;
    int AutoscanInterval;
    struct {
      bool Using;
      std::string Path;
    } PatchFile;
    struct {
      bool Enabled;
      std::string InDesc;
      std::string OutDesc;
    } ControlPort;
    struct {
      bool Enabled;
      std::string InDesc;
      std::string OutDesc;
      bool StayConnected;
    } RemoteControl;
} Options = {
  .Verbosity = 0,
  .ShowUpdates = false,
  .AutoscanInterval = 1000,
  .PatchFile = {
    .Using = false,
    .Path = ""
  },
  .ControlPort = {
    .Enabled = false,
    .InDesc = "VirtMidiIn:MidiPatcher-Control",
    .OutDesc = "VirtMidiOut:MidiPatcher-Control"
  },
  .RemoteControl = {
    .Enabled = false,
    .InDesc = "MidiIn:MidiPatcher-Control",
    .OutDesc = "MidiOut:MidiPatcher-Control",
    .StayConnected = false
  }
};

class NotificationHandler : public virtual MidiPatcher::PortRegistry::PortRegistryUpdateReceiver {

    // void deviceDiscovered(MidiPatcher::AbstractPort * port){
    //     std::cout << "DISCOVERED " << port->getKey() << std::endl;
    // }

    void portRegistered( MidiPatcher::AbstractPort * port ){
      if (Options.ShowUpdates == false){
        return;
      }
      MidiPatcher::PortDescriptor * desc = port->getPortDescriptor();
      std::cout << "REGISTERED " << desc->toString() << std::endl;
      delete desc;
    }

    void portUnregistered( MidiPatcher::AbstractPort * port ){
      if (Options.ShowUpdates == false){
        return;
      }
      MidiPatcher::PortDescriptor * desc = port->getPortDescriptor();
      std::cout << "UNREGISTERED " << desc->toString() << std::endl;
      delete desc;
    }

    void deviceStateChanged(MidiPatcher::AbstractPort * port, MidiPatcher::AbstractPort::DeviceState_t newState){
      if (Options.ShowUpdates == false){
        return;
      }
      std::cout << "DEVSTATE " << port->getKey() << " " << newState << std::endl;
    }

    void portsConnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport ){
      if (Options.ShowUpdates == false){
        return;
      }
      std::cout << "CONNECTED " << inport->getKey() << " " << outport->getKey() << std::endl;
    }

    void portsDisconnected( MidiPatcher::AbstractPort * inport, MidiPatcher::AbstractPort * outport ){
      if (Options.ShowUpdates == false){
        return;
      }
      std::cout << "DISCONNECTED " << inport->getKey() << " " << outport->getKey() << std::endl;
    }
};

NotificationHandler * notificationHandler;

/***************************/

void SignalHandler(int signal);
void setupSignalHandler( void );

void logger(MidiPatcher::Log::Level_t level, std::string message, unsigned char * bytes = nullptr, size_t len = 0);

void printVersion( void );
void printHelp( void );
void listPorts( void );
void listPortClasses( void );

int setupPortsFromArgs(int argc, char * argv[]);
int setupPortsFromFile(std::string file);

void setupControlPort( void );

void remoteControl(int argc, char * argv[]);
void remoteControlReceived(unsigned char * data, int len, MidiPatcher::Port::InjectorPort * injectorPort, void * userData);

void init();
void deinit();

/***************************/

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


void SignalHandler(int signal)
{
  // if (SIG)

  if (Running){
    // Attempt to gracefully stop process.
    Running = false;
    // std::cerr << "Press CTRL-C again to quit." << std::endl;
    return;
  }

  // Force quit if necessary
  std::exit(EXIT_FAILURE);
}

void setupSignalHandler(){
  // sigintTicks = 0;
  std::signal(SIGINT, SignalHandler);
}

void logger(MidiPatcher::Log::Level_t level, std::string message, unsigned char * bytes, size_t len){

  bool printed = false;

  switch(level){
    case MidiPatcher::Log::NOTICE:
      if (Options.Verbosity >= 1){
        std::cerr << "NOTICE " << message;
        printed = true;
      }
      break;

    case MidiPatcher::Log::INFO:
      if (Options.Verbosity >= 2){
        std::cerr << "INFO " << message;
        printed = true;
      }
      break;

    case MidiPatcher::Log::DEBUG:
      if (Options.Verbosity >= 3){
        std::cerr << "DEBUG " << message;
        printed = true;
      }
      break;

    case MidiPatcher::Log::WARNING:
      std::cerr << "WARNING " << message;
      printed = true;
      break;

    case MidiPatcher::Log::ERROR:
      std::cerr << "ERROR " << message;
      printed = true;
      break;

    default:
      std::cerr << "LOG (" << std::to_string(level) << ") " << message;
      printed = true;
      break;
  }

  if (printed){

    if (len > 0){
      assert( bytes != nullptr );

      std::cerr << ", data (" << std::dec << len << ") ";
      for(size_t i = 0; i < len; i++){
        std::cerr << std::setfill('0') << std::setw(2) << std::hex << (int)bytes[i] << ' ';
      }
    }

    std::cerr << std::endl;
  }
}

void printVersion(){
    std::cout << MidiPatcher::VERSION << std::endl;
}

void printHelp() {

    // https://stackoverflow.com/a/25021520/1982142

    std::cout <<
    #include "main.help.in"
    << std::endl;

    printf("midipatcher %s, MIT license, https://github.com/tschiemer/midipatcher\n", MidiPatcher::VERSION.c_str());
}


void listPorts(){

  init();

  portRegistry->rescan();

  std::vector<MidiPatcher::AbstractPort*> * ports = portRegistry->getAllPorts();

  std::cout << ports->size() << std::endl;

  std::for_each(ports->begin(), ports->end(), [](MidiPatcher::AbstractPort* port){
    MidiPatcher::PortDescriptor * desc = port->getPortDescriptor();
    std::cout << desc->toString() << std::endl;
    delete desc;
  });

  delete ports;
}

void listPortClasses(){

  init();

  std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> * list = portRegistry->getPortClassRegistryInfoList();

  std::cout << list->size() << std::endl;

  for(int i = 0; i < list->size(); i++){
    std::cout << list->at(i)->Key << std::endl;
  }

  delete list;
}


int setupPortsFromArgs(int argc, char * argv[]){

  std::vector<MidiPatcher::AbstractPort*> inports = std::vector<MidiPatcher::AbstractPort*>();
  std::vector<MidiPatcher::AbstractPort*> outports = std::vector<MidiPatcher::AbstractPort*>();

  for (int i = 0, j = 1; i < argc; i += 2, j += 2){

    MidiPatcher::PortDescriptor * desc;
    MidiPatcher::AbstractPort * port;

    try {

      desc = MidiPatcher::PortDescriptor::fromString(argv[i]);
      port = portRegistry->registerPortFromDescriptor(desc);
      inports.push_back(port);

      desc = MidiPatcher::PortDescriptor::fromString(argv[j]);
      port = portRegistry->registerPortFromDescriptor(desc);
      outports.push_back(port);

    } catch (std::exception &e){
      std::cerr << "ERROR " << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  assert( inports.size() == outports.size() );

  for( int i = 0; i < inports.size(); i++){
    portRegistry->connectPorts( inports.at(i), outports.at(i) );
  }

  return inports.size();
}

int setupPortsFromFile(std::string file){

  std::vector<MidiPatcher::AbstractPort*> inports = std::vector<MidiPatcher::AbstractPort*>();
  std::vector<MidiPatcher::AbstractPort*> outports = std::vector<MidiPatcher::AbstractPort*>();

  std::ifstream patchfile;
  patchfile.open(file);

  // expected format:
  // <in-port-desc> ;; <out-port-desc>

  std::string line;
  while(getline(patchfile, line)){
    trim(line);

    int pos;
    std::string in, out;

    pos = line.find("#");

    if (pos != std::string::npos){
      line.erase(pos);
    }

    if (line.size() == 0){
      continue;
    }

    pos = line.find("\t");

    if (pos == std::string::npos){
      std::cerr << "ERROR no tab separating in-/out-port-descriptors on line: " << line << std::endl;
      exit(EXIT_FAILURE);
    }

    in = line.substr(0,pos);
    trim(in);

    line.erase(0,pos+1);
    out = line;
    trim(out);

    // std::cout << "[" << in << "] [" << out << "]" << std::endl;

    try {

      MidiPatcher::PortDescriptor * desc;
      MidiPatcher::AbstractPort * port;

      desc = MidiPatcher::PortDescriptor::fromString(in);
      port = portRegistry->registerPortFromDescriptor(desc);
      inports.push_back(port);

      desc = MidiPatcher::PortDescriptor::fromString(out);
      port = portRegistry->registerPortFromDescriptor(desc);
      outports.push_back(port);

    } catch (std::exception &e){
      std::cerr << "ERROR " << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  }


  assert( inports.size() == outports.size() );

  for( int i = 0; i < inports.size(); i++){
    portRegistry->connectPorts( inports.at(i), outports.at(i) );
  }

  return inports.size();
}

void setupControlPort(){
  MidiPatcher::PortDescriptor * desc;
  MidiPatcher::AbstractPort * inport, * outport;

  desc = MidiPatcher::PortDescriptor::fromString(Options.ControlPort.InDesc);
  inport = portRegistry->registerPortFromDescriptor(desc);
  delete desc;

  desc = MidiPatcher::PortDescriptor::fromString(Options.ControlPort.OutDesc);
  outport = portRegistry->registerPortFromDescriptor(desc);
  delete desc;

  MidiPatcher::Port::ControlPort * cp = new MidiPatcher::Port::ControlPort(portRegistry);
  portRegistry->registerPort( cp );

  portRegistry->connectPorts(inport, cp);
  portRegistry->connectPorts(cp, outport);
}

void remoteControl(int argc, char * argv[]){

  portRegistry->rescan();

  MidiPatcher::PortDescriptor * desc;
  MidiPatcher::AbstractPort * inport, * outport;

  desc = MidiPatcher::PortDescriptor::fromString(Options.RemoteControl.InDesc);
  inport = portRegistry->registerPortFromDescriptor(desc);
  delete desc;

  desc = MidiPatcher::PortDescriptor::fromString(Options.RemoteControl.OutDesc);
  outport = portRegistry->registerPortFromDescriptor(desc);
  delete desc;

  MidiPatcher::Port::InjectorPort * ip = new MidiPatcher::Port::InjectorPort("RemoteControl", remoteControlReceived);
  //[](unsigned char * data, int len, MidiPatcher::Port::InjectorPort * injectorPort, void * userData){

  // });

  portRegistry->registerPort( ip );

  portRegistry->connectPorts(inport, ip);
  portRegistry->connectPorts(ip, outport);

  // TODO wait for connections..

  // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  if (argc > 0){
    std::vector<std::string> command;

    for(int i = 0; i < argc; i++){
      command.push_back(argv[i]);
    }

    unsigned char midi[128];

    int len = MidiPatcher::Port::ControlPort::packMessage( midi, command );

    // std::cout << "remote (" << len << ") ";

    // for(int i = 0; i < len; i++){
    //   std::cout << std::hex << (int)midi[i] << " ";
    // }
    // std::cout << std::endl;

    assert( len > 0 );

    ip->send( midi, len );
  }

  setupSignalHandler();

  Running = true;
  while(Running){
    // wait
  }
}

void remoteControlReceived(unsigned char * data, int len, MidiPatcher::Port::InjectorPort * injectorPort, void * userData){
  // std::cout << "Received (" << len << ")" << std::endl;

  std::vector<std::string> response;

  MidiPatcher::Port::ControlPort::unpackMessage(response, data, len);

  if (response.size() == 0){
    return;
  }

  for(int i = 0; i < response.size(); i++){
    std::cout << response[i] << " ";
  }
  std::cout << std::endl;

  if (Options.RemoteControl.StayConnected == false){
    if (response[0] == "OK" || response[0] == "ERROR"){
      Running = false;
    }
  }
}

void init(){

  MidiPatcher::Log::registerLogger(logger);

  std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> pcriList;

  assert( sizeof(PortClassRegistryInfoConfig) > 0 );

  for(int i = 0; i < sizeof(PortClassRegistryInfoConfig) / sizeof(MidiPatcher::AbstractPort::PortClassRegistryInfo*) ; i++){
      pcriList.push_back( PortClassRegistryInfoConfig[i] );
  }

  portRegistry = new MidiPatcher::PortRegistry(pcriList);

  portRegistry->init();

  std::atexit(deinit);

  notificationHandler = new NotificationHandler();
  portRegistry->subscribePortRegistryUpdateReveicer(notificationHandler);
}

void deinit(){

  portRegistry->unsubscribePortRegistryUpdateReveicer(notificationHandler);

  portRegistry->deinit();

  delete portRegistry;
}

void runloop(){
  portRegistry->runloop();
}

int main(int argc, char * argv[], char * env[]){

  int c;

  if (argc <= 1){
      printHelp();
      return EXIT_SUCCESS;
  }

    while (1) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;

        static struct option long_options[] = {
          {"version", no_argument, 0, 'v'},
          {"help", no_argument, 0, 'h'},
          {"ports", no_argument, 0, 'p'},
          {"autoscan", required_argument, 0, 'a'},

          {"pc", no_argument, 0, 1},
          {"port-classes", no_argument, 0, 1},
          // {"persist", no_argument, 0, 'p'},

          // {"pf", required_argument, 0, 'f'},
          {"patch-file", required_argument, 0, 'f'},

          {"cp", no_argument, 0, 2},
          {"control-port", no_argument, 0, 2},
          {"cp-in", required_argument, 0, 3},
          {"control-port-in", required_argument, 0, 3},
          {"cp-out", required_argument, 0, 4},
          {"control-port-out", required_argument, 0, 4},

          {"remote", no_argument, 0, 'r'},
          {"remote-in", required_argument, 0, 5},
          {"remote-out", required_argument, 0, 6},

          {0,0,0,0}
        };

        c = getopt_long(argc, argv, "vh?dua:pf:r",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            // case 0:

            case 1:
              listPortClasses();
              return EXIT_SUCCESS;

            case 2:
                Options.ControlPort.Enabled = true;
                break;

            case 3:
                Options.ControlPort.InDesc = optarg;
                break;

            case 4:
                Options.ControlPort.OutDesc = optarg;
                break;

            case 'r':
                Options.RemoteControl.Enabled = true;
                break;

            case 5:
                Options.RemoteControl.InDesc = optarg;
                break;

            case 6:
                Options.RemoteControl.OutDesc = optarg;
                break;

            case 'v':
                printVersion();
                return EXIT_SUCCESS;

            case '?':
            case 'h':
                printHelp();
                return EXIT_SUCCESS;

            case 'd':
                Options.Verbosity++;
                break;

            case 'u':
                Options.ShowUpdates = true;
                break;

            case 'p':
                listPorts();
                return EXIT_SUCCESS;

            case 'a':
                Options.AutoscanInterval = std::atoi(optarg);
                if (Options.AutoscanInterval < 0){
                  std::cerr << "ERROR invalid autscan value " << Options.AutoscanInterval << std::endl;
                  return EXIT_FAILURE;
                }
                break;

            case 'f':
                // if (optarg == NULL){
                //   std::cerr << "ERROR patch file option without file" << std::endl;
                //   return EXIT_FAILURE;
                // }
                Options.PatchFile.Using = true;
                Options.PatchFile.Path = optarg;
                break;

            default:
                printf("?? getopt returned character code %o ??\n", c);
        }

    }


    init();

    int argC = argc - optind;
    char ** argV = &argv[optind];

    if (Options.RemoteControl.Enabled){

      if (argC <= 0){
        Options.RemoteControl.StayConnected = true;
      //   std::cerr << "ERROR Missing remote commands" << std::endl;
      //   return EXIT_FAILURE;
      }

      remoteControl(argC, argV);

      return EXIT_SUCCESS;
    }


    // validate possible argument listing of in-/out-port patching
    if (argC > 0){
      if (argC % 2 == 1){
        std::cerr << "ERROR port list must always be duplets of input-port and output-port" << std::endl;
        return EXIT_FAILURE;
      }
    }


    if (Options.ControlPort.Enabled){
      setupControlPort();
    }

    int portCount = 0;

    if (Options.PatchFile.Using){
      portCount += setupPortsFromFile(Options.PatchFile.Path);
    }

    if (argC > 0){
      portCount += setupPortsFromArgs(argC, argV);
    }

    // no ports were actually set up and control port was not enabled -> no reason to actually run.
    if (portCount == 0 && Options.ControlPort.Enabled == false){
      std::cerr << "ERROR neither have any ports been configured nor are you using a control port to dynamically change the setup" << std::endl;
      return EXIT_FAILURE;
    }

    portRegistry->rescan();

    if (Options.AutoscanInterval > 0){
      portRegistry->enableAutoscan(Options.AutoscanInterval);
    }

    setupSignalHandler();

    // std::cout << "Processing... quit by pressing CTRL-C twice." << std::endl;
    Running = true;
    while(Running){
      runloop();
    }

    return EXIT_SUCCESS;
}
