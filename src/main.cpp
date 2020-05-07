
#include <iostream>
#include <getopt.h>
#include <csignal>

#include <map>

#include <RtMidi.h>

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

std::vector< std::string * > * getInputPorts(){

    std::vector< std::string *> * result = new std::vector< std::string* >();

    RtMidiIn *midiin = 0;

    try {

      midiin = new RtMidiIn();

      // Check inputs.
      unsigned int nPorts = midiin->getPortCount();

      for ( unsigned i=0; i<nPorts; i++ ) {
        result->push_back( new std::string(midiin->getPortName(i)) ) ;
      }

    } catch ( RtMidiError &error ) {
      error.printMessage();
    }

    delete midiin;

    return result;
}


std::vector< std::string * > * getOutputPorts(){

    std::vector< std::string *> * result = new std::vector< std::string* >();

    RtMidiOut *midiout = 0;

    try {

      midiout = new RtMidiOut();

      // Check inputs.
      unsigned int nPorts = midiout->getPortCount();

      for ( unsigned i=0; i<nPorts; i++ ) {
        result->push_back( new std::string(midiout->getPortName(i)) ) ;
      }

    } catch ( RtMidiError &error ) {
      error.printMessage();
    }

    delete midiout;

    return result;
}

void listInterfaces(bool listInputPorts = true, bool listOutputPorts = true){

  RtMidiIn  *midiin = 0;
  RtMidiOut *midiout = 0;

  try {

    if (listInputPorts){

      std::vector< std::string * > *list = getInputPorts();

      std::cout << list->size() << std::endl;

      for(int i = 0; i < list->size(); i++){
        std::cout << i << " " << *list->at(i) << std::endl;
      }

      delete list;
    }

    if (listOutputPorts){
        std::vector< std::string * > *list = getOutputPorts();

        std::cout << list->size() << std::endl;

        for(int i = 0; i < list->size(); i++){
          std::cout << i << " " << *list->at(i) << std::endl;
        }

        delete list;
    }

  } catch ( RtMidiError &error ) {
    error.printMessage();
  }
}

int main(int argc, char * argv[], char * env[]){


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
          {"list-interfaces", optional_argument, 0, 'l'},
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
              if (optarg != NULL && strlen(optarg) > 0){
                if (strlen(optarg) == 1 && optarg[0] == 'i'){
                  listInterfaces(true,false);
                } else if (strlen(optarg) == 1 && optarg[0] == 'o'){
                  listInterfaces(false,true);
                } else {
                  std::cerr << "ERROR invalid option for --list-interfaces" << std::endl;
                }
              } else {
                listInterfaces(true, true);
              }
              return EXIT_SUCCESS;

            case 'i':
              std::cout << "opt-i" <<std::endl;
              break;

            case 'o':
              std::cout << "opt-o" << std::endl;
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


            RtMidiOut * midiout;

            try {
              midiout = new RtMidiOut;

              midiout->openPort(0, "To Max 2");

              // if (midiout->isPortOpen()){
                std::cout << "is open? " << (midiout->isPortOpen() ? "yes" : "no") << std::endl;
                std::cout << midiout->getPortName() << std::endl;
              // }

              midiout->closePort();
            } catch(RtMidiError &error ) {
              error.printMessage();
            }

            delete midiout;


    return EXIT_SUCCESS;
}
