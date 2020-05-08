
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

void printHelp( void ) {
    printf("Usage:\n");
    printf("\t rme-totalmix-midi-adapter [-h?]\n");
    printf("\nOptions:\n");
    printf("\t -h|-? \t\t\t\t show this help\n");
    printf("\t -l|--list[i|o] list [input/output] ports\n");
    printf("\n");
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


void listInterfaces(){

  portRegistry->rescan();

  std::vector<MidiPatcher::AbstractPort*> * ports = portRegistry->getAllPorts();

  std::for_each(ports->begin(), ports->end(), [](MidiPatcher::AbstractPort* port){
    std::cout << "[" << port->getKey() << "] " << port->Name << std::endl;
  });

}

void deinit();

void init(){

    MidiPatcher::PortRegistry::init();

    portRegistry = new MidiPatcher::PortRegistry();

    std::atexit(deinit);
}

void deinit(){

  delete portRegistry;

  MidiPatcher::PortRegistry::deinit();

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
          // {"control-surface", required_argument, 0, 'c'},
          // {"session-name", optional_argument, 0, 's'},
          {"list-interfaces", no_argument, 0, 'l'},
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

        c = getopt_long(argc, argv, "h?l::io",
                        long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            // case 0:

            case '?':
            case 'h':
                printHelp();
                return EXIT_SUCCESS;

            case 'l':
              // if (optarg != NULL && strlen(optarg) > 0){
              //   if (strlen(optarg) == 1 && optarg[0] == 'i'){
              //     listInterfaces(true,false);
              //   } else if (strlen(optarg) == 1 && optarg[0] == 'o'){
              //     listInterfaces(false,true);
              //   } else {
              //     std::cerr << "ERROR invalid option for --list-interfaces" << std::endl;
              //   }
              // } else {
                listInterfaces();
              // }
              return EXIT_SUCCESS;

            case 'i':
              assert( optarg != NULL && strlen(optarg) > 0 );

              std::cout << "opt-i " << optarg << std::endl;
              break;

            case 'o':
              assert( optarg != NULL && strlen(optarg) > 0 );

              std::cout << "opt-o " << optarg << std::endl;
              break;

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

    char ** args = &argv[optind];


    std::vector<MidiPatcher::AbstractPort*> * inports = new std::vector<MidiPatcher::AbstractPort*>();
    std::vector<MidiPatcher::AbstractPort*> * outports = new std::vector<MidiPatcher::AbstractPort*>();

    // important..
    portRegistry->rescan();

    for (int i = 0, j = 1; i < arg; i += 2, j += 2){
      MidiPatcher::AbstractPort * port;

      port = portRegistry->findPortByName(args[i], MidiPatcher::AbstractPort::TypeInput);
      if (port == NULL){
        std::cerr << "ERROR input port '" << args[i] << "' not found." << std::endl;
        return EXIT_FAILURE;
      }
      inports->push_back(port);

      port = portRegistry->findPortByName(args[j], MidiPatcher::AbstractPort::TypeOutput);
      if (port == NULL){
        std::cerr << "ERROR output port '" << args[j] << "' not found." << std::endl;
        return EXIT_FAILURE;
      }
      outports->push_back(port);

    }

    assert( inports->size() == outports->size() );

    setupSignalHandler();

    for( int i = 0; i < inports->size(); i++){
      portRegistry->connectPorts( inports->at(i), outports->at(i) );
    }

    delete inports;
    delete outports;

    std::cout << "Processing... quit by pressing CTRL-C twice." << std::endl;
    Running = true;
    while(Running){
      // wait...
    }

    return EXIT_SUCCESS;
}
