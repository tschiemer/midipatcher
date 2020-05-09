#ifndef PORT_PIPE_IN
#define PORT_PIPE_IN


#include <AbstractInputPort.hpp>


namespace MidiPatcher {
  namespace Port {

    // class PipeIn : AbstractInputPort {
    //
    //   public:
    //
    //     static void init(){
    //       // do nothing
    //     }
    //
    //     static void deinit(){
    //       // do nothing
    //     }
    //
    //     static const constexpr char * Key = "PipeIn";
    //
    //     std::string getKey(){
    //       return Key;
    //     }
    //
    //
    //     static AbstractPort* factory(PortRegistry * portRegistry, PortDescriptor * portDescriptor);
    //
    //     static PortFamilyDeclaration * getDeclaration(){
    //       return new PortFamilyDeclaration(Key, init, deinit, NULL, factory);
    //     }
    //
    //
    //   protected:
    //
    //     void reader();
    //
    //     void addConnectionImpl(AbstractPort * port);
    //
    //     void removeConnectionImpl(AbstractPort * port);
    //
    // }

  }
}

#endif
