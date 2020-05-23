#include <PortRegistry.hpp>

#include <Port/AbstractPort.hpp>
#include <Port/AbstractInputPort.hpp>
#include <Port/AbstractOutputPort.hpp>

#include <log.hpp>

#include <cassert>
#include <utility>

#include <iostream>

#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif


namespace MidiPatcher {

  PortRegistry::PortRegistry(std::vector<AbstractPort::PortClassRegistryInfo *> &pcriList){

      for (int i = 0; i < pcriList.size(); i++){
         PortClassRegistryInfoMap[pcriList[i]->Key] = pcriList[i];
      }
  }

  PortRegistry::~PortRegistry(){
    disableAutoscan();

    // unregister all subscribers
    PortRegistryUpdateReceiverList.clear();
    // while(PortRegistryUpdateReceiverList.size() > 0){
    //   PortRegistryUpdateReceiverList
    // }

    // properly delete all connections
    std::for_each(Ports.begin(), Ports.end(), [this](std::pair<std::string,AbstractPort *> p){

      AbstractInputPort * inport;

      // port = p.second;

      inport = dynamic_cast<AbstractInputPort*>(p.second);
      if ( inport == nullptr ){
        return;
      }

      std::vector<AbstractPort *> * connections = inport->getConnections();

      std::for_each(connections->begin(), connections->end(), [this,inport](AbstractPort* other){

        AbstractOutputPort * outport = dynamic_cast<AbstractOutputPort*>(other);
        if (outport == nullptr){
          return;
        }

        disconnectPorts(dynamic_cast<AbstractPort*>(inport), dynamic_cast<AbstractPort*>(outport));

      });

      delete connections;
    });


    std::for_each(Ports.begin(), Ports.end(), [this](std::pair<std::string,AbstractPort *> p){
      delete p.second;
    });
    Ports.clear();

    std::for_each(PortClassRegistryInfoMap.begin(),PortClassRegistryInfoMap.end(), [](std::pair<std::string,AbstractPort::PortClassRegistryInfo *> pair){
      delete pair.second;
    });


  }


  void PortRegistry::init(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); it++)
    {
      if (it->second->Init != NULL){
        it->second->Init();
      }
    }
  }

  void PortRegistry::deinit(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); it++)
    {
      if (it->second->Deinit != NULL){
        it->second->Deinit();
      }
    }

  }

  void PortRegistry::runloop(){
    #ifdef __APPLE__
    CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, false);
    #endif
  }

  std::vector<AbstractPort::PortClassRegistryInfo*> * PortRegistry::getPortClassRegistryInfoList(){

    std::vector<AbstractPort::PortClassRegistryInfo*> * list = new std::vector<AbstractPort::PortClassRegistryInfo*>();

    for( std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); it++){
      list->push_back( it->second );
    }

    return list;
  }

  void PortRegistry::rescan(){

    for (std::map<std::string, AbstractPort::PortClassRegistryInfo*>::iterator it = PortClassRegistryInfoMap.begin(); it != PortClassRegistryInfoMap.end(); ++it)
    {
      // std::cout << "Scanning " << it->first << std::endl;
      if (it->second->Scanner != NULL){
        std::vector<AbstractPort*>  * list = it->second->Scanner(this);

        for(int i = 0; i < list->size(); i++){
          AbstractPort * port = list->at(i);
          if (getPortByKey(port->getKey()) == NULL){
            port->registerPort( *this );
          }
        }

        delete list;
      }
    }

  }

  std::vector<AbstractPort*> * PortRegistry::getAllPorts(){
    std::vector<AbstractPort*> * ports = new std::vector<AbstractPort*>();

    std::for_each(Ports.begin(), Ports.end(), [&ports](std::pair<std::string,AbstractPort*> p){
      ports->push_back(p.second);
    });

    return ports;
  }

  AbstractPort * PortRegistry::getPortByKey(std::string key){
    if (Ports.count(key) > 0){
      return Ports[key];
    }
    return NULL;
  }

  AbstractPort * PortRegistry::getPortById( unsigned int id ){
    AbstractPort * result = NULL;

    for(std::map<std::string,AbstractPort*>::iterator it = Ports.begin(); result == NULL && it != Ports.end(); it++){
      if (it->second->Id == id){
        result = it->second;
      }
    }

    return result;
  }

  void PortRegistry::registerPort(AbstractPort * port){

    std::string key = port->getKey();

    // std::cout << "registerPort? [" << key << "]" << std::endl;

    // do not re-register port with identical descriptor
    if (Ports.count(key) > 0){
      return;
    }

    port->Id = ++EntryIncrement;
    Ports[key] = port;

    // std::cout << "YES " << Ports.size() <<  std::endl;

    Log::print(0, "registerPort( " + port->getKey() + " )");

    publishPortRegistered( port );

    port->subscribePortUpdateReveicer( this );
  }

  void PortRegistry::unregisterPort(AbstractPort * port){
    std::string key = port->getKey();

    if (Ports.count(key) == 0){
      return;
    }

    Log::print(0, "unregisterPort( " + port->getKey() + " )");

    port->unsubscribePortUpdateReveicer( this );

    std::vector<AbstractPort *> * connections = port->getConnections();

    // remove all connections
    std::for_each(connections->begin(), connections->end(), [port](AbstractPort*other){
      // removeConnection(port, other);
      port->removeConnection(other);
      other->removeConnection(port);
    });

    delete connections;

    // should it be deleted? naah.
    // delete port;

    publishPortUnregistered( port );

  }

  AbstractPort* PortRegistry::registerPortFromDescriptor(PortDescriptor * portDescriptor){
    assert( portDescriptor != NULL );
    if ( portClassExists(portDescriptor->PortClass) == false ){
      throw "PortClass " + portDescriptor->PortClass + " does not exist";
    }

    if (Ports.count(portDescriptor->getKey()) > 0){
      // std::cout << "exists! " << portDescriptor->getKey() << std::endl;
      return Ports[portDescriptor->getKey()];
    }
    // std::cout << "doesn't exist! " << portDescriptor->getKey() << std::endl;

    AbstractPort * port = PortClassRegistryInfoMap[portDescriptor->PortClass]->Factory(portDescriptor);

    registerPort(port);

    return port;
  }


  void PortRegistry::connectPorts(AbstractPort *input, AbstractPort *output){
    assert(input != NULL);
    assert(output != NULL);

    Log::print(0, "connectPort( " + input->getKey() + ", " + output->getKey() + " )");

    // std::cout << "Connecting [" << input->Name << "] -> [" << output->Name << "]" << std::endl;

    output->addConnection(input);
    input->addConnection(output);

    publishPortsConnected(input,output);
  }

  void PortRegistry::connectPortsByKey(std::string inputKey, std::string outputKey){
    assert( getPortByKey(inputKey) != NULL );
    assert( getPortByKey(outputKey) != NULL );

    connectPorts( getPortByKey(inputKey), getPortByKey(outputKey) );
  }

  void PortRegistry::disconnectPorts(AbstractPort *input, AbstractPort *output){
    assert(input != NULL);
    assert(output != NULL);

    Log::print(0, "disconnectPorts( " + input->getKey() + ", " + output->getKey() + " )");

    output->removeConnection(input);
    input->removeConnection(output);

    publishPortsDisconnected(input,output);
  }

  void PortRegistry::disconnectPortsByKey(std::string inputKey, std::string outputKey){
    assert( getPortByKey(inputKey) != NULL );
    assert( getPortByKey(outputKey) != NULL );

    connectPorts( getPortByKey(inputKey), getPortByKey(outputKey) );
  }

  void PortRegistry::enableAutoscan(unsigned int intervalMsec){
    if (AutoscanEnabled == true){
      return;
    }

    setAutoscanInterval(intervalMsec);

    AutoscanEnabled = true;

    AutoscanThread = std::thread([this]() {
      while (AutoscanEnabled)
      {
        this->rescan();
        std::this_thread::sleep_for(std::chrono::milliseconds(AutoscanIntervalMsec));
      }
    });
    // AutoscanThread.detach();
  }

  void PortRegistry::disableAutoscan(){
    AutoscanEnabled = false;
    AutoscanThread.join();
  }

  // void PortRegistry::deviceDiscovered(AbstractPort * port){
  //
  //   publishDeviceDiscovered(port);
  //
  // }

  void PortRegistry::deviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState){

    Log::print(0, "deviceStateChanged( " + port->getKey() + " " + std::to_string(newState) + " )");

    publishDeviceStateChanged(port, newState);
  }


  // void PortRegistry::publishDeviceDiscovered(AbstractPort * port){
  //   std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [port](PortUpdateReceiver* receiver){
  //     receiver->deviceDiscovered( port );
  //   });
  // }

  void PortRegistry::publishPortRegistered(AbstractPort * port){
    std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [port](PortRegistryUpdateReceiver* receiver){
      receiver->portRegistered( port );
    });
  }

  void PortRegistry::publishPortUnregistered(AbstractPort * port){
    std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [port](PortRegistryUpdateReceiver* receiver){
      receiver->portUnregistered( port );
    });
  }

  void PortRegistry::publishDeviceStateChanged(AbstractPort * port, AbstractPort::DeviceState_t newState){
    std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [port, newState](PortRegistryUpdateReceiver* receiver){
      receiver->deviceStateChanged( port, newState );
    });
  }

  void PortRegistry::publishPortsConnected(  AbstractPort * inport, AbstractPort * outport ){
    std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [inport,outport](PortRegistryUpdateReceiver* receiver){
      receiver->portsConnected( inport, outport );
    });
  }

  void PortRegistry::publishPortsDisconnected(  AbstractPort * inport, AbstractPort * outport ){
    std::for_each(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), [inport,outport](PortRegistryUpdateReceiver* receiver){
      receiver->portsDisconnected( inport, outport );
    });
  }


  void PortRegistry::subscribePortRegistryUpdateReveicer(PortRegistryUpdateReceiver *receiver){
    PortRegistryUpdateReceiverList.push_back(receiver);
  }

  void PortRegistry::unsubscribePortRegistryUpdateReveicer(PortRegistryUpdateReceiver *receiver){
    PortRegistryUpdateReceiverList.erase(std::remove(PortRegistryUpdateReceiverList.begin(), PortRegistryUpdateReceiverList.end(), receiver));
  }

}
