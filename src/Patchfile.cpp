#include <Patchfile.hpp>

#include <PortRegistry.hpp>
#include <Port/AbstractPort.hpp>
#include <Port/AbstractInputPort.hpp>
#include <Port/AbstractOutputPort.hpp>

#include <Log.hpp>
#include <Error.hpp>

#include <fstream>

namespace MidiPatcher {
  namespace Patchfile {

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

    int loadFromPatchfile(PortRegistry * portRegistry, std::string fname){

        std::vector<MidiPatcher::AbstractPort*> inports = std::vector<MidiPatcher::AbstractPort*>();
        std::vector<MidiPatcher::AbstractPort*> outports = std::vector<MidiPatcher::AbstractPort*>();

        std::ifstream patchfile;
        patchfile.open(fname);

        // expected format:
        // <in-port-desc> ;; <out-port-desc>

        int lineN = 0;

        std::string line;
        while(getline(patchfile, line)){

          lineN++;

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
            new Error("no tab separating in-/out-port-descriptors on line: " + std::to_string(lineN));
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
            // new Error(e);
            throw e;
          }
        }


        assert( inports.size() == outports.size() );

        for( int i = 0; i < inports.size(); i++){
          try {
            portRegistry->connectPorts( dynamic_cast<MidiPatcher::AbstractInputPort*>(inports.at(i)), dynamic_cast<MidiPatcher::AbstractOutputPort*>(outports.at(i)) );
          } catch( MidiPatcher::Error &e){
            new Error(std::string(e.what()) + ": " + inports.at(i)->getKey() + " <-> " + outports.at(i)->getKey());
          }
        }

        return inports.size();
    }

    void saveToPatchfile(PortRegistry * portRegistry, std::string fname){

    }

  }
}
