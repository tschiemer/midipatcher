#include "main.hpp"

#include <iostream>
#include <getopt.h>
#include <csignal>
#include <cstdlib>

#include <map>

#include <RtMidi.h>


#include <PortRegistry.hpp>


MidiPatcher::PortRegistry * portRegistry = NULL;

int sigintTicks = 0;
volatile bool Running = false;

void printVersion( void ){
    std::cout << "midipatcher " << VERSION << std::endl;
}

void printHelp( void ) {
    printf("Usage:\n");
    printf("\t midipatcher [-vh?]\n");
    printf("\t midipatcher (-l|--list-ports)\n");
    printf("\t midipatcher (-p|--list-port-classes)\n");
    printf("\t midipatcher <in-descriptor1> <out-descriptor1> ... \n");
    printf("\nOptions:\n");
    printf("\t -v \t\t\t\t Show version.\n");
    printf("\t -h|-? \t\t\t\t Show this help.\n");
    printf("\t -l|--list-ports \t\t List known/detected ports (descriptors).\n");
    printf("\t -p|--list-port-classes \t List registered port classes.\n");
    printf("\n");
    printf("<in-/out-descriptor> := <port-key>[<options>]\n");
    printf("<port-key> := <PortClass>:<PortName>\n");
    printf("<options> := (,<opt> ...)\n");
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
    printf("\n");
    printf("Thanks to:\n\n");
    printf("RtMidi: realtime MIDI i/o C++ classes, http://www.music.mcgill.ca/~gary/rtmidi\n");
    printf("Asio (Networking) C++ Library, https://think-async.com/Asio\n");
    printf("\n");
    printf("MIT license, https://github.com/tschiemer/midipatcher\n");
}

void SignalHandler(int signal)
{
  // if (SIG)
  sigintTicks++;

  if (sigintTicks == 1){
    std::cerr << "Press CTRL-C again to quit." << std::endl;
    return;
  }

  // Attempt to gracefully stop process.
  Running = false;

  // Force quit if necessary
  if (sigintTicks > 2){
    exit(EXIT_FAILURE);
  }
}

void setupSignalHandler(){
  sigintTicks = 0;
  std::signal(SIGINT, SignalHandler);
}


void listInterfaces(bool printCount = true, bool printList = true){

  portRegistry->rescan();

  std::vector<MidiPatcher::AbstractPort*> * ports = portRegistry->getAllPorts();

  if (printCount){
    std::cout << ports->size() << std::endl;
  }

  if (printList){
    std::for_each(ports->begin(), ports->end(), [](MidiPatcher::AbstractPort* port){
      MidiPatcher::PortDescriptor * desc = port->getPortDescriptor();
      std::cout << desc->toString() << std::endl;
      delete desc;
    });
  }

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

void deinit();

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
          {"list-ports", no_argument, 0, 'l'},
          {"list-port-classes", no_argument, 0, 'p'},
                // {"parse",    no_argument,    0,  'p' },
                // {"generate",   no_argument,    0,  'g' },
                // {"timed", optional_argument,  0, 't'},
                // {"running-status", optional_argument, 0, 'r'},
                // {"prefix", required_argument, 0, 0},
                // {"suffix", required_argument, 0, 0},
                // {"verbose", optional_argument, 0, 'v'},
                // {"exit-on-error", required_argument, 0, 'x'},
                // {"help",    no_argument,    0,  'h' },
                // {"convert", required_argument, 0, 0},
                // {"hex", no_argument, 0, 0},
                // {"nrpn-filter", no_argument, 0, 'n'},
            {0,         0,              0,  0 }
        };

        c = getopt_long(argc, argv, "vh?lp",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            // case 0:

            case 'v':
                printVersion();
                return EXIT_SUCCESS;

            case '?':
            case 'h':
                printHelp();
                return EXIT_SUCCESS;

            case 'l':
                listInterfaces();
                return EXIT_SUCCESS;

            case 'p':
                listPortClasses();
                return EXIT_SUCCESS;

            // case 'i':
            //   assert( optarg != NULL && strlen(optarg) > 0 );
            //
            //   std::cout << "opt-i " << optarg << std::endl;
            //   break;
            //
            // case 'o':
            //   assert( optarg != NULL && strlen(optarg) > 0 );
            //
            //   std::cout << "opt-o " << optarg << std::endl;
            //   break;

            // case 'c':
            //   if (optarg != NULL && strlen(optarg) > 0){
            //     for (int i = 0; i < ControlSurfaceCount; i++){
            //       if (ControlSurfaceList[i].Key == optarg){
            //         controlSurfaceFactory = ControlSurfaceList[i].Factory;
            //         std::cout << "Control Surface: " << ControlSurfaceList[i].Key << std::endl;
            //       }
            //     }
            //   }
            //   break;
            //
            // case 's':
            //   if (optarg != NULL && strlen(optarg) > 0){
            //     sessionName = optarg;
            //
            //     std::cout << "Session Name: " << sessionName << std::endl;
            //   }
            //   break;


            default:
                printf("?? getopt returned character code %o ??\n", c);
        }

    }

    int arg = argc - optind;

    if (arg <= 0){
      std::cerr << "ERROR Missing ports to connect!" << std::endl;
      return EXIT_FAILURE;
    }
    if (arg % 2 == 1){
      std::cerr << "ERROR port list must always be duplets of input-port and output-port" << std::endl;
      return EXIT_FAILURE;
    }

    portRegistry->rescan();

    char ** args = &argv[optind];


    std::vector<MidiPatcher::AbstractPort*> * inports = new std::vector<MidiPatcher::AbstractPort*>();
    std::vector<MidiPatcher::AbstractPort*> * outports = new std::vector<MidiPatcher::AbstractPort*>();

    // important..
    //
    // portRegistry->rescan();
    //
    // for (int i = 0, j = 1; i < arg; i += 2, j += 2){
    //   MidiPatcher::AbstractPort * port;
    //
    //   port = portRegistry->findPortByName(args[i], MidiPatcher::AbstractPort::TypeInput);
    //   if (port == NULL){
    //     std::cerr << "ERROR input port '" << args[i] << "' not found." << std::endl;
    //     return EXIT_FAILURE;
    //   }
    //   inports->push_back(port);
    //
    //   port = portRegistry->findPortByName(args[j], MidiPatcher::AbstractPort::TypeOutput);
    //   if (port == NULL){
    //     std::cerr << "ERROR output port '" << args[j] << "' not found." << std::endl;
    //     return EXIT_FAILURE;
    //   }
    //   outports->push_back(port);
    //
    // }

    for (int i = 0, j = 1; i < arg; i += 2, j += 2){

      MidiPatcher::PortDescriptor * desc;
      MidiPatcher::AbstractPort * port;

      desc = MidiPatcher::PortDescriptor::fromString(args[i]);
      port = portRegistry->registerPortFromDescriptor(desc);
      inports->push_back(port);

      desc = MidiPatcher::PortDescriptor::fromString(args[j]);
      port = portRegistry->registerPortFromDescriptor(desc);
      outports->push_back(port);
    }

        // portRegistry->rescan();

    assert( inports->size() == outports->size() );

    setupSignalHandler();

    for( int i = 0; i < inports->size(); i++){
      portRegistry->connectPorts( inports->at(i), outports->at(i) );
    }

    delete inports;
    delete outports;


        // portRegistry->rescan();

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    portRegistry->enableAutoscan(1000);

    // listInterfaces();

    std::cout << "Processing... quit by pressing CTRL-C twice." << std::endl;
    Running = true;
    while(Running){
      // wait...
    }

    return EXIT_SUCCESS;
}
