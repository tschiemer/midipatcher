#include "main.hpp"

#include <iostream>
#include <fstream>
#include <getopt.h>
#include <csignal>
#include <cstdlib>

#include <map>

/***************************/

MidiPatcher::PortRegistry * portRegistry = nullptr;

MidiPatcher::CLI * cli = nullptr;

volatile bool Running = false;


enum ControlType_t {
  ControlTypeNone,
  ControlTypeInteractive,
  ControlTypeControlPort,
  ControlTypeRemote,
  ControlTypeTcp
};

struct {
    int Verbosity;
    bool ShowUpdates;
    int AutoscanInterval;
    ControlType_t ControlType;
    std::string CLIDelimiter;
    std::vector<std::string> ControlBlockedClasses;
    struct {
      std::string Path;
      bool LoadOnStart;
      bool SaveOnExit;
    } PatchFile;
    struct {
      unsigned short Port;
      std::string Password;
    } TCPControl;
    struct {
      std::string InDesc;
      std::string OutDesc;
    } ControlPort;
    struct {
      std::string InDesc;
      std::string OutDesc;
    } RemoteControl;
} Options = {
  .Verbosity = 0,
  .ShowUpdates = false,
  .AutoscanInterval = DEFAULT_AUTOSCAN_INTERVAL,
  .ControlType = ControlTypeNone,
  .CLIDelimiter = DEFAULT_CLI_DELIMITER,
  .PatchFile = {
    .Path = DEFAULT_PATCHFILE,
    .LoadOnStart = false,
    .SaveOnExit = false
  },
  .TCPControl = {
    // .Port =
    .Password = ""
  },
  .ControlPort = {
    .InDesc = DEFAULT_CONTROL_PORT_IN,
    .OutDesc = DEFAULT_CONTROL_PORT_OUT
  },
  .RemoteControl = {
    .InDesc = DEFAULT_REMOTE_CONTROL_PORT_IN,
    .OutDesc = DEFAULT_REMOTE_CONTROL_PORT_OUT
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

bool isClassKnown(std::string pc);
bool isClassBlocked(std::string pc);

void setOptionControlType(ControlType_t controlType);

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
void setupRemoteControlPort( void );

// void remoteControl(int argc, char * argv[]);
// void remoteControlReceived(unsigned char * data, int len, MidiPatcher::Port::InjectorPort * injectorPort, void * userData);

void init();
void deinit();
void runloop();

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

bool isClassKnown(std::string pc){
  for(int i = 0; i < sizeof(PortClassRegistryInfoConfig) / sizeof(MidiPatcher::AbstractPort::PortClassRegistryInfo*) ; i++){
    if (PortClassRegistryInfoConfig[i]->Key == pc){
      return true;
    }
  }
  return false;
}

bool isClassBlocked(std::string pc){
  std::vector<std::string>::iterator found = std::find(Options.ControlBlockedClasses.begin(), Options.ControlBlockedClasses.end(), pc);
  return found != Options.ControlBlockedClasses.end();
}

void setOptionControlType(ControlType_t controlType){
  if (Options.ControlType != ControlTypeNone){
    std::cerr << "ERROR multiple control types (-i, -r, --tcpc, --cp) defined, only one possible!" << std::endl;
    exit(EXIT_FAILURE);
  }
  Options.ControlType = controlType;
}

void SignalHandler(int signal)
{
  // if (SIG)

  if (Running){
    // Attempt to gracefully stop process.
    Running = false;

    // needed for interactiveControl
    fclose(stdin);
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

    printf("%s, MIT license, https://github.com/tschiemer/midipatcher\n", MidiPatcher::APPLICATION.c_str());
}


void listPorts(){

  init();

  portRegistry->rescan();

  std::vector<MidiPatcher::AbstractPort*> * ports = portRegistry->getAllPorts();

  std::cout << ports->size() << std::endl;

  std::for_each(ports->begin(), ports->end(), [](MidiPatcher::AbstractPort* port){
    MidiPatcher::PortDescriptor * desc = port->getPortDescriptor();
    std::cout << port->getId() << " " << port->getType() << " " << port->getDeviceState() << " " << desc->toString() << std::endl;
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
      port = portRegistry->registerPortFromDescriptor(*desc);
      inports.push_back(port);

      desc = MidiPatcher::PortDescriptor::fromString(argv[j]);
      port = portRegistry->registerPortFromDescriptor(*desc);
      outports.push_back(port);

    } catch (std::exception &e){
      std::cerr << "ERROR " << e.what() << std::endl;
      std::exit(EXIT_FAILURE);
    }
  }

  assert( inports.size() == outports.size() );

  for( int i = 0; i < inports.size(); i++){
    try {
      portRegistry->connectPorts( dynamic_cast<MidiPatcher::AbstractInputPort*>(inports.at(i)), dynamic_cast<MidiPatcher::AbstractOutputPort*>(outports.at(i)) );
    } catch( MidiPatcher::Error &e){
      std::cerr << "ERROR " << e.what() << ": " << inports.at(i)->getKey() << " <-> " << outports.at(i)->getKey() << std::endl;
    }
  }

  return inports.size();
}

int setupPortsFromFile(std::string file){


  int loaded = 0;
  try {
    loaded = MidiPatcher::Patchfile::loadFromPatchfile(portRegistry, file);
  } catch( std::exception &e){
    std::cerr << "ERROR " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }

  MidiPatcher::Log::notice("Loaded " + std::to_string(loaded) + " connections from " + file);

  return loaded;
}

void setupControlPort(){

  MidiPatcher::PortDescriptor * desc;
  MidiPatcher::AbstractInputPort * inport;
  MidiPatcher::AbstractOutputPort * outport;

  desc = MidiPatcher::PortDescriptor::fromString(Options.ControlPort.InDesc);
  inport = dynamic_cast<MidiPatcher::AbstractInputPort*>(portRegistry->registerPortFromDescriptor(*desc));
  delete desc;

  desc = MidiPatcher::PortDescriptor::fromString(Options.ControlPort.OutDesc);
  outport = dynamic_cast<MidiPatcher::AbstractOutputPort*>(portRegistry->registerPortFromDescriptor(*desc));
  delete desc;

  MidiPatcher::Port::ControlPort * cp = new MidiPatcher::Port::ControlPort(portRegistry);
  portRegistry->registerPort( cp );

  portRegistry->connectPorts(inport, cp);
  portRegistry->connectPorts(cp, outport);
}

void setupRemoteControlPort( void ){

  portRegistry->rescan();

  MidiPatcher::PortDescriptor * desc;
  MidiPatcher::AbstractInputPort * inport;
  MidiPatcher::AbstractOutputPort * outport;

  desc = MidiPatcher::PortDescriptor::fromString(Options.RemoteControl.InDesc);
  inport = dynamic_cast<MidiPatcher::AbstractInputPort*>(portRegistry->registerPortFromDescriptor(*desc));
  delete desc;

  desc = MidiPatcher::PortDescriptor::fromString(Options.RemoteControl.OutDesc);
  outport = dynamic_cast<MidiPatcher::AbstractOutputPort*>(portRegistry->registerPortFromDescriptor(*desc));
  delete desc;

  MidiPatcher::Port::RemoteControlPort * remote = new MidiPatcher::Port::RemoteControlPort();

  portRegistry->registerPort( remote );

  portRegistry->connectPorts(inport, remote);
  portRegistry->connectPorts(remote, outport);

  cli = (MidiPatcher::CLI*)remote;
}

void init(){

  static bool once = true;

  if (once){
    once = false;
  } else {
    return;
  }

  MidiPatcher::Log::registerLogger(logger);

  std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> pcriList;

  assert( sizeof(PortClassRegistryInfoConfig) > 0 );

  for(int i = 0; i < sizeof(PortClassRegistryInfoConfig) / sizeof(MidiPatcher::AbstractPort::PortClassRegistryInfo*) ; i++){
    if (isClassBlocked(PortClassRegistryInfoConfig[i]->Key)){
      continue;
    }
    pcriList.push_back( PortClassRegistryInfoConfig[i] );
  }

  portRegistry = new MidiPatcher::PortRegistry(pcriList);

  std::atexit(deinit);

  notificationHandler = new NotificationHandler();
  portRegistry->subscribePortRegistryUpdateReveicer(notificationHandler);
}

void deinit(){


  if (Options.ControlType == ControlTypeTcp){
    MidiPatcher::TCPControl::stop();
  }

  portRegistry->unsubscribePortRegistryUpdateReveicer(notificationHandler);

  if (cli != nullptr){
    if (Options.ControlType == ControlTypeInteractive){
      delete dynamic_cast<MidiPatcher::InteractiveControl*>(cli);
    }
    if (Options.ControlType == ControlTypeRemote){
      // delete dynamic_cast<MidiPatcher::Port::RemoteControlPort*>(cli);
    }
  }

  if (Options.PatchFile.SaveOnExit){
    MidiPatcher::Patchfile::saveToPatchfile(portRegistry, Options.PatchFile.Path);
  }

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
          {"patch-file", optional_argument, 0, 'f'},
          {"los", no_argument, 0, 9},
          {"load-on-start", no_argument, 0, 9},
          {"sox", no_argument, 0, 10},
          {"save-on-exit", no_argument, 0, 10},

          {"interactive", no_argument, 0, 'i'},
          {"delimiter", required_argument, 0, 8},

          {"tcpc", required_argument, 0, 't'},
          {"tcp-control", required_argument, 0, 't'},
          {"tcp-pass", required_argument, 0, 7},

          {"cp", no_argument, 0, 2},
          {"control-port", no_argument, 0, 2},
          {"cp-in", required_argument, 0, 3},
          {"control-port-in", required_argument, 0, 3},
          {"cp-out", required_argument, 0, 4},
          {"control-port-out", required_argument, 0, 4},

          {"remote", no_argument, 0, 'r'},
          {"remote-in", required_argument, 0, 5},
          {"remote-out", required_argument, 0, 6},

          {"no", required_argument, 0, 11},


          {0,0,0,0}
        };

        c = getopt_long(argc, argv, "vh?dua:plf:rit:",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            // case 0:

            case 1:
              listPortClasses();
              return EXIT_SUCCESS;

            case 2:
                setOptionControlType(ControlTypeControlPort);
                break;

            case 3:
                Options.ControlPort.InDesc = optarg;
                break;

            case 4:
                Options.ControlPort.OutDesc = optarg;
                break;

            case 'r':
                setOptionControlType(ControlTypeRemote);
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
            case 'l':
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
                Options.PatchFile.LoadOnStart = true;

                if (optarg != nullptr){
                  Options.PatchFile.Path = optarg;
                }
                break;

            case 9:
                Options.PatchFile.LoadOnStart = true;
                break;

            case 10:
                Options.PatchFile.SaveOnExit = true;
                break;

            case 'i':
                setOptionControlType(ControlTypeInteractive);
                break;

            case 't':
              setOptionControlType(ControlTypeTcp);
              Options.TCPControl.Port = std::atoi(optarg);
              break;

            case 7:
              Options.TCPControl.Password = optarg;
              break;

            case 8:
              Options.CLIDelimiter = optarg;
              break;

            case 11:
              if (isClassKnown(optarg) == false){
                std::cerr << "ERROR forbidden class " << optarg << " is not known" << std::endl;
                exit(EXIT_FAILURE);
              }
              Options.ControlBlockedClasses.push_back(optarg);
              break;


            default:
                printf("?? getopt returned character code %o ??\n", c);
        }

    }


    init();

    int argC = argc - optind;
    char ** argV = &argv[optind];

    // validate possible argument listing of in-/out-port patching
    if (argC > 0){
      if (argC % 2 == 1){
        std::cerr << "ERROR port list must always be duplets of input-port and output-port" << std::endl;
        return EXIT_FAILURE;
      }
    }



    // setup controls before processing patch setup as ControlPort type might be somehow patched..

    if (Options.ControlType == ControlTypeInteractive){
      cli = (MidiPatcher::CLI*)new MidiPatcher::InteractiveControl(portRegistry);
    }

    if (Options.ControlType == ControlTypeControlPort){
      setupControlPort();
    }

    if (Options.ControlType == ControlTypeRemote){
      setupRemoteControlPort();
    }

    if (cli != nullptr){
      cli->setArgvDelimiter(Options.CLIDelimiter);
    }

    if (Options.ControlType == ControlTypeTcp){
      MidiPatcher::TCPControl::init(portRegistry, Options.TCPControl.Port, Options.TCPControl.Password);
      std::thread([](){
        MidiPatcher::TCPControl::start();
      }).detach();
      MidiPatcher::TCPControl::setDefaultArgvDelimiter(Options.CLIDelimiter);
    }

    int portCount = 0;

    if (Options.PatchFile.LoadOnStart){
      portCount += setupPortsFromFile(Options.PatchFile.Path);
    }

    if (argC > 0){
      portCount += setupPortsFromArgs(argC, argV);
    }

    // no ports were actually set up and control port was not enabled -> no reason to actually run.
    if (portCount == 0 && Options.ControlType == ControlTypeNone){
      std::cerr << "ERROR neither have any ports been configured nor are you using a method to to dynamically change the setup" << std::endl;
      return EXIT_FAILURE;
    }

    setupSignalHandler();

    portRegistry->rescan();

    if (Options.AutoscanInterval > 0){
      portRegistry->setAutoscanInterval(Options.AutoscanInterval);
      portRegistry->startAutoscan();
    }

    // std::cout << "Processing... quit by pressing CTRL-C twice." << std::endl;
    Running = true;
    while(Running){
      runloop();

      if (Options.ControlType == ControlTypeInteractive || Options.ControlType == ControlTypeRemote){
        // explicitly check for false condition (do NOT set Running w/o check as might override previous termination condition)
        if (cli->runloop() == false){
          Running = false;
        }
      }
    }

    return EXIT_SUCCESS;
}
