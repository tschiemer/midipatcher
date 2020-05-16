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
    } RemoteControl;
} Options = {
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
    .OutDesc = "MidiOut:MidiPatcher-Control"
  }
};


/***************************/

void printVersion( void );
void printHelp( void );
void listPorts();
void listPortClasses();

void setupPortsFromArgs(int argc, char * argv[]);
void setupPortsFromFile(std::string file);

void setupControlPort();
void setupRemoteControl();

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



void printVersion( void ){
    std::cout << MidiPatcher::VERSION << std::endl;
}

void printHelp( void ) {

    // https://stackoverflow.com/a/25021520/1982142

    std::cout <<
    #include "main.help.in"
    << std::endl;

    printf("midipatcher %s, MIT license, https://github.com/tschiemer/midipatcher\n", MidiPatcher::VERSION.c_str());
}

// void SignalHandler(int signal)
// {
//   // if (SIG)
//   sigintTicks++;
//
//   if (sigintTicks == 1){
//     std::cerr << "Press CTRL-C again to quit." << std::endl;
//     return;
//   }
//
//   // Attempt to gracefully stop process.
//   Running = false;
//
//   // Force quit if necessary
//   if (sigintTicks > 2){
//     exit(EXIT_FAILURE);
//   }
// }
//
// void setupSignalHandler(){
//   sigintTicks = 0;
//   std::signal(SIGINT, SignalHandler);
// }


void listPorts(){

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
  std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> * list = portRegistry->getPortClassRegistryInfoList();

  std::cout << list->size() << std::endl;

  for(int i = 0; i < list->size(); i++){
    std::cout << list->at(i)->Key << std::endl;
  }

  delete list;
}


void setupPortsFromArgs(int argc, char * argv[]){

  std::vector<MidiPatcher::AbstractPort*> inports = std::vector<MidiPatcher::AbstractPort*>();
  std::vector<MidiPatcher::AbstractPort*> outports = std::vector<MidiPatcher::AbstractPort*>();

  for (int i = 0, j = 1; i < argc; i += 2, j += 2){

    MidiPatcher::PortDescriptor * desc;
    MidiPatcher::AbstractPort * port;

    desc = MidiPatcher::PortDescriptor::fromString(argv[i]);
    port = portRegistry->registerPortFromDescriptor(desc);
    inports.push_back(port);

    desc = MidiPatcher::PortDescriptor::fromString(argv[j]);
    port = portRegistry->registerPortFromDescriptor(desc);
    outports.push_back(port);
  }

  assert( inports.size() == outports.size() );

  for( int i = 0; i < inports.size(); i++){
    portRegistry->connectPorts( inports.at(i), outports.at(i) );
  }

}

void setupPortsFromFile(std::string file){

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


    MidiPatcher::PortDescriptor * desc;
    MidiPatcher::AbstractPort * port;

    desc = MidiPatcher::PortDescriptor::fromString(in);
    port = portRegistry->registerPortFromDescriptor(desc);
    inports.push_back(port);

    desc = MidiPatcher::PortDescriptor::fromString(out);
    port = portRegistry->registerPortFromDescriptor(desc);
    outports.push_back(port);
  }


  assert( inports.size() == outports.size() );

  for( int i = 0; i < inports.size(); i++){
    portRegistry->connectPorts( inports.at(i), outports.at(i) );
  }

  // exit(EXIT_SUCCESS);
}

void setupControlPort(){
    std::cerr << "TODO Control Port" << std::endl;
}

void setupRemoteControl(){
    std::cerr << "TODO Remote Control" << std::endl;
}

void init(){

  std::vector<MidiPatcher::AbstractPort::PortClassRegistryInfo*> pcriList;

  assert( sizeof(PortClassRegistryInfoConfig) > 0 );

  for(int i = 0; i < sizeof(PortClassRegistryInfoConfig) / sizeof(MidiPatcher::AbstractPort::PortClassRegistryInfo*) ; i++){
      pcriList.push_back( PortClassRegistryInfoConfig[i] );
  }

  portRegistry = new MidiPatcher::PortRegistry(pcriList);

  portRegistry->init();

  std::atexit(deinit);
}

void deinit(){

  portRegistry->deinit();

  delete portRegistry;
}

int main(int argc, char * argv[], char * env[]){

  init();

  int c;
  int digit_optind = 0;

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

        c = getopt_long(argc, argv, "vh?a:pf:r",
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
                return EXIT_SUCCESS;

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


    int argC = argc - optind;
    char ** argV = &argv[optind];

    if (Options.RemoteControl.Enabled){

      if (argC <= 0){
        std::cerr << "ERROR Missing remote commands" << std::endl;
      }

      setupRemoteControl();



      return EXIT_SUCCESS;
    }


    // validate possible argument listing of in-/out-port patching
    if (argC > 0){
      if (argC % 2 == 1){
        std::cerr << "ERROR port list must always be duplets of input-port and output-port" << std::endl;
        return EXIT_FAILURE;
      }
    }

    portRegistry->rescan();

    if (Options.ControlPort.Enabled){
      setupControlPort();
    }

    if (Options.PatchFile.Using){
      setupPortsFromFile(Options.PatchFile.Path);
    }

    if (argC > 0){
      setupPortsFromArgs(argC, argV);
    }


    if (Options.AutoscanInterval > 0){
      portRegistry->enableAutoscan(Options.AutoscanInterval);
    }


    // std::cout << "Processing... quit by pressing CTRL-C twice." << std::endl;
    Running = true;
    while(Running){
      // wait...
    }

    return EXIT_SUCCESS;
}
