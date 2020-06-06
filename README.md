# midipatcher
Platform independent matrix patchbay for MIDI ports with additional connectivity for virtual ports and file, serial(work in progress), network streams, rtpmidi (work in progress).

https://github.com/tschiemer/midipatcher

## Guide to (Mostly) Harmless Command Line Interfacing

```bash
Usage:
	 midipatcher [-vh?]
	 midipatcher (-l|--ports)
	 midipatcher (--pc|--port-classes)

	 midipatcher [-d[d[d]]][-a<autoscan-interval>] [-u] [<control-type>] [<patch-options>]

	 	<control-type>
			(-i|--interactive)
			(--tcpc|--tcp-control) [--tcp-pass <password>]
			(--cp|--control-port) [--cp-in <control-in-port-descriptor>] [--cp-out <control-out-port-descriptor>]
			(-r|--remote) [--remote-in <remote-control-in-port-descriptor>] [--remote-out <remote-control-out-port-descriptor>] <remote-command...>

		<patch-options>
		  [-f|patch-file <patch-file>] [<in-port-descriptor1> <out-port-descriptor1> ... ]

Options:
	 -v 				 Show version.
	 -h|-? 				 Show this help.
	 -a|--autoscan <autoscan-interval>
					 Set regular rescanning of ports with given interval in millisec (default 1000; 0 = off).
	 -p|--ports 			 List known/detected ports (descriptors).
	 --pc|--port-classes 		 List registered port classes.
	 -d			Increase debug level by one.
	 -u			Show Updates.

	 -f|--patch-file <patch-file> 	 Use <patch-file> for patching configuration

	 -i|--interactive
					 Control from CLI

					 Commands:

					 ping
					 		Connection test
					 version
					 		Get midipatcher version
					 option <option-key> [<option-value>]
					 		Get/set open
							<option-key> := return-ids 	return port-id instead of port-key
					 portclasses
					 		Get list of available port classes
					 ports [<port-id>|<port-key>]
					 		Get basic port listing of given or all port(s); response contains port-id and port-key
					 devstate [<port-id>|<port-key>]
					 		Get DeviceState of given or all port(s) (is device connected/enabled or not?)
					 register <port-descriptor>
					 		Register new port using given descriptor
					 unregister (<port-id>|<port-key>)
					 		Unregister port
					 constate [(<port1-id>|<port1-key>) [(<port2-id>|<port2-key>)]]
					    	Get connection state between input- and output-port; either query specific connection, or list connections of specific/all input ports
					 connect (<in-port-id>|<in-port-key>) (<in-port-id>|<out-port-key>)
					 		Connect two (registered) ports
					 disconnect (<in-port-id>|<in-port-key>) (<in-port-id>|<out-port-key>)
					 		Disconnect two ports

	 --tcpc|--tcp-control <port>
	 				Create a TCP port on which to listen for commands.
					NOTE: By default no password is required! (see --tcp-pass).
					Uses same commands as interactive control.
					Upon connection, clients will be greeted by the application with name and version.

	 --tcp-pass <password>
	 				Use given password to authenticate connections.
					When a password is required, clients are prompted to provide it before being able to send any commands.
					NOTE: (at the moment) the connection is insecure, ie not encrypted. Thus just a very basic level of security is given.

	 --cp|--control-port
	 --cp-in|--control-in-port <control-in-port-descriptor>
	 --cp-out|--control-out-port <control-out-port-descriptor>
					 Use control port with default options: "--cp-in VirtMidiIn:MidiPatcher-Control --cp-out VirtMidiOut:MidiPatcher-Control".

	 -r|--remote ... <remote-command>
	 --remote-in <remote-control-in-port-descriptor>
	 --remote-out <remote-control-out-port-descriptor>
					 Act as remote control with default port options "--remote-in MidiOut:MidiPatcher-Control --remote-out MidiIn:MidiPatcher-Control".
					 Same commands as -i|--interactive


	 <in-/out-/control-port-descriptor> := <port-key>[<options>]
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
	 RawExec:(<exec>|<name>,exec=<exec>)[,argv=<argv>]
	 MsgExec:(<exec>|<name>,exec=<exec>)[,argv=<argv>]

The <port-key> part of the descriptors acts as unique port identifier.

Basic examples:

midipatcher MidiIn:BCF2000 "MidiOut:Yamaha 01V96 Port1"
midipatcher "MidiIn:from Max 1" VirtMidiOut:p1 MidiIn:p1 VirtMidiOut:p2 MidiIn:p2 "MidiOut:to Max 1"
midipatcher "MidiIn:from Max 1" VirtMidiOut:ComboPort "MidiIn:from Max 2" VirtMidiOut:ComboPort
midipatcher MidiIn:BCF2000 UdpOut:10.0.0.4:3000 UdpIn:3001 MidiOut:BCF2000
midipatcher UdpIn:3000 "VirtMidiOut:From other computer" "VirtMidiIn:To other computer" UdpOut:10.0.0.2:3001

Control examples:
midipatcher -i
midipatcher --tcpc 9900 --tcp-pass secure & socat - TCP-CONNECT:localhost:9900
midipatcher --cp & midipatcher -r ping
midipatcher --cp & midipatcher -r

More, fancy examples:
midimessage -g | midipatcher FileIn:STDIN RawExec:examples/RawExec/inc-channel RawExec:examples/RawExec/inc-channel FileOut:STDOUT | midimessage -p
midimessage -g | midipatcher FileIn:STDIN MsgExec:/bin/cat MsgExec:/bin/cat FileOut:STDOUT | midimessage -p
midimessage -g | midipatcher FileIn:STDIN MsgExec:my-inc,exec=examples/MsgExec/inc-cc-channel.sh MsgExec:my-inc FileOut:STDOUT | midimessage -p

Thanks to:

RtMidi: realtime MIDI i/o C++ classes, http://www.music.mcgill.ca/~gary/rtmidi
Asio (Networking) C++ Library, https://think-async.com/Asio
midimessage: midimessage library, https://github.com/tschiemer/midimessage

midipatcher-v0.1.0-48-gc2538fc, MIT license, https://github.com/tschiemer/midipatcher
```

## Patchfiles

Parsing rules:

- Anything behind a hashtag (#) is considered a comment and is ignored
- Empty lines are allowed
- Any mapping MUST have exactly two port descriptions separated by one or more tabs
- If a port occurs multiple times you MUST provide the full port description including options in the first occurance; options are not requried in any following occurrances.

Example:
```
# This file contains comments
MidiIn:from Max 1							MidiOut:to Max 1 		# looping back from max 1 to max 1 :|
MidiIn:from Max 2					MidiOut:to Max 1		# almost the same as above


# Patchfiles are loaded after the control port is created, thus, you can consider it in your patchfile
ControlPort:Default					MidiOut:to Max 2
```



## Roadmap / Todos

- Test, bugfix and complete current source base
	- Introduce consistent error/exception system
- Add examples
	- Using Max/MSP and PureData matrices in combination with control port
- Integrate RTPMIDI
- Add serial port (using asio)
- Add case insensitivity of portclasses (for creation descriptors) (?)
- Get/set port-specific options after creation (where sensible) (?)


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

and use the convenience include header `<midipatcher/midipatcher.hpp>` in any source files.

## License

MIT License (also see `LICENSE` file).

Thanks to (and licensing according to component):

- [RtMidi: realtime MIDI i/o C++ classes](http://www.music.mcgill.ca/~gary/rtmidi)
- [Asio (Networking) C++ Library](https://think-async.com/Asio)
- [midimessage: midimessage library](https://github.com/tschiemer/midimessage)
