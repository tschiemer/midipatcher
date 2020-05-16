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

void printVersion( void ){
    std::cout << MidiPatcher::VERSION << std::endl;
}

void printHelp( void ) {
    printf("Usage:\n");
    printf("\t midipatcher [-vh?]\n");
    printf("\t midipatcher (-l|--ports)\n");
    printf("\t midipatcher (--pc|--port-classes)\n");
    printf("\n");
    printf("\t midipatcher [--cp|--control-port] [--cp-in <control-in-port-descriptor>] [--cp-out <control-out-port-descriptor>] ...\n");
    printf("\t midipatcher [-f|patch-file <patch-file>] ...\n");
    printf("\t midipatcher ... [<in-port-descriptor1> <out-port-descriptor1> ... ]\n");
    printf("\n");
    printf("\t midipatcher (-r|--remote) [--remote-in <remote-control-in-port-descriptor>] [--remote-out <remote-control-out-port-descriptor>] <remote-command...>\n");

    printf("\nOptions:\n");
    printf("\t -v \t\t\t\t Show version.\n");
    printf("\t -h|-? \t\t\t\t Show this help.\n");
    printf("\t -a|--autoscan <autoscan-interval> \t Set regular rescanning of ports with given interval in millisec (default 1000; 0 = off).\n");
    printf("\t -p|--ports \t\t\t List known/detected ports (descriptors).\n");
    printf("\t --pc|--port-classes \t\t List registered port classes.\n");
    printf("\n");
    // printf("\t -p|--persist \t\t\t Enable port persistance, ie do not exit when a port is or becomes unavailable.\n");
    printf("\t -f|--patch-file <patch-file> \t Use <patch-file> for patching configuration\n");
    printf("\n");
    printf("\t --cp|--control-port\n");
    printf("\t --cp-in|--control-in-port <control-in-port-descriptor>\n");
    printf("\t --cp-out|--control-out-port <control-out-port-descriptor>\n");
    printf("\t\t\t\t\t Use control port with default options: \"--cp-in VirtMidiIn:MidiPatcher-Control --cp-out VirtMidiOut:MidiPatcher-Control\".\n");
    printf("\n");
    printf("\t -r|--remote ... <remote-command>\n");
    printf("\t --remote-in <remote-control-in-port-descriptor>\n");
    printf("\t --remote-out <remote-control-out-port-descriptor>\n");
    printf("\t\t\t\t\t Act as remote control with default port options \"--remote-in MidiOut:MidiPatcher-Control --remote-out MidiIn:MidiPatcher-Control\".\n");
    printf("\n\t\t\t\t\t connect <in-port-descriptor> <out-port-descriptor> ...\n");
    printf("\t\t\t\t\t disconnect <in-port-descriptor> <out-port-descriptor> ...\n");
    printf("\t\t\t\t\t dump\n");
    printf("\t\t\t\t\t\t Dumps connection\n");
    printf("\n");
    printf("\t <in-/out-/control-port-descriptor> := <port-key>[<options>]\n");
    printf("\t <port-key> := <PortClass>:<PortName>\n");
    printf("\t <options> := (,<opt> ...)\n");
    printf("\n");
    printf("\t MidiIn:<PortName>, \t Connects (or waits for) MIDI port with given name\n");
    printf("\t MidiOut:<PortName>\n");
    printf("\t VirtMidiIn:<PortName> \t Create virtual MIDI port with given name\n");
    printf("\t VirtMidiOut:<PortName>\n");
    printf("\t FileIn:(STDIN|<Filename>)[,runningstatus=(0|1*)]\n");
    printf("\t FileOut:(STDOUT|STDERR|<Filename>)[,runningstatus=(0*|1)]\n");
    printf("\t\t Opens said file for read/writing. Wether MIDI running status is enabled can be set (* = default)\n");
    printf("\t UdpIn:[<listen-addr>:]<port>[,multicast=<multicast-addr>][,runningstatus=(0|1*)]\n");
    printf("\t UdpOut:<port>[,runningstatus=(0*|1)]\n");
    printf("\n");
    printf("The <port-key> part of the descriptors acts as unique port identifier.\n");
    printf("\n");
    printf("Examples:\n\n");
    printf("midipatcher MidiIn:BCF2000 \"MidiOut:Yamaha 01V96 Port1\"\n");
    printf("midipatcher \"MidiIn:from Max 1\" VirtMidiOut:p1 MidiIn:p1 VirtMidiOut:p2 MidiIn:p2 \"MidiOut:to Max 1\"\n");
    printf("midipatcher \"MidiIn:from Max 1\" VirtMidiOut:ComboPort \"MidiIn:from Max 2\" VirtMidiOut:ComboPort\n");
    printf("midipatcher MidiIn:BCF2000 UdpOut:10.0.0.4:3000 UdpIn:3001 MidiOut:BCF2000\n");
    printf("midipatcher UdpIn:3000 \"VirtMidiOut:From other computer\" \"VirtMidiIn:To other computer\" UdpOut:10.0.0.2:3001\n");
    printf("midipatcher FileIn:STDIN RawExec:examples/RawExec/inc-channel RawExec:examples/RawExec/inc-channel FileOut:STDOUT\n");
    printf("\n");
    printf("Thanks to:\n\n");
    printf("RtMidi: realtime MIDI i/o C++ classes, http://www.music.mcgill.ca/~gary/rtmidi\n");
    printf("Asio (Networking) C++ Library, https://think-async.com/Asio\n");
    printf("\n");
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

      // portRegistry->rescan();

  assert( inports.size() == outports.size() );

  // setupSignalHandler();

  for( int i = 0; i < inports.size(); i++){
    portRegistry->connectPorts( inports.at(i), outports.at(i) );
  }

  // delete inports;
  // delete outports;
}

void setupPortsFromFile(std::string file){
  std::ifstream patchfile;
  patchfile.open(file);

  // expected format:
  // <in-port-desc> ;; <out-port-desc>

  std::string line;
  while(getline(patchfile, line)){
    std::cerr << "read " << line.size() << " " << line << std::endl;
  }
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
