# midipatcher
Matrix patchbay for MIDI ports with additional connectivity for virtual ports and file, serial, network streams.

https://github.com/tschiemer/midipatcher

## Guide to (Mostly) Harmless Command Line Interfacing

```bash
Usage:
	 midipatcher [-vh?]
	 midipatcher (-l|--list-ports)
	 midipatcher (-p|--list-port-classes)
	 midipatcher <in-descriptor1> <out-descriptor1> ...

Options:
	 -v 				 Show version.
	 -h|-? 				 Show this help.
	 -l|--list-ports 		 List known/detected ports (descriptors).
	 -p|--list-port-classes 	 List registered port classes.

<in-/out-descriptor> := <port-key>[<options>]
<port-key> := <PortClass>:<PortName>
<options> := (,<opt> ...)

	 MidiIn:<PortName>, 	 Connects (or waits for) MIDI port with given name
	 MidiOut:<PortName>
	 VirtMidiIn:<PortName> 	 Create virtual MIDI port with given name
	 VirtMidiOut:<PortName>
	 FileIn:(STDIN|<Filename>)[,runningstatus=(0|1*)]
	 FileOut:(STDOUT|STDERR|<Filename>)[,runningstatus=(0*|1)]
		 Opens said file for read/writing. Wether MIDI running status is enabled can be set (* = default)
	 UdpIn:[<listen-addr>:]<port>[,multicast=<multicast-addr>][,runningstatus=(0|1*)]
	 UdpOut:<port>[,runningstatus=(0*|1)]

The <port-key> part of the descriptors acts as unique port identifier.

Examples:

midipatcher MidiIn:BCF2000 "MidiOut:Yamaha 01V96 Port1"
midipatcher "MidiIn:from Max 1" VirtMidiOut:p1 MidiIn:p1 VirtMidiOut:p2 MidiIn:p2 "MidiOut:to Max 1"
midipatcher "MidiIn:from Max 1" VirtMidiOut:ComboPort "MidiIn:from Max 2" VirtMidiOut:ComboPort
midipatcher MidiIn:BCF2000 UdpOut:10.0.0.4:3000 UdpIn:3001 MidiOut:BCF2000
midipatcher UdpIn:3000 "VirtMidiOut:From other computer" "VirtMidiIn:To other computer" UdpOut:10.0.0.2:3001

Thanks to:

RtMidi: realtime MIDI i/o C++ classes, http://www.music.mcgill.ca/~gary/rtmidi
Asio (Networking) C++ Library, https://think-async.com/Asio

MIT license (https://github.com/tschiemer/midipatcher)
```

## Building

```bash
git clone --recursive https://github.com/tschiemer/midipatcher
cd midipatcher
cmake .
make

```

## Using as library

The project uses cmake, thus simply do something as follows:

```
set(MIDIPATCHER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/midipatcher)

add_subdirectory(${MIDIPATCHER_DIR} EXCLUDE_FROM_ALL)
set(MIDIPATCHER_INCLUDES ${MIDIPATCHER_DIR}/include)

add_executable(myProject ...)
target_include_directories(myProject "${MIDIPATCHER_INCLUDES}")
target_link_libraries(myProject midipatch)
```

and use the convenience include header `include/midipatcher/midipatcher.hpp` in any source files.

## License

MIT License (also see `LICENSE` file).
