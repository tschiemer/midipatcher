# midipatcher
Platform independent virtual matrix patchbay for MIDI ports with additional connectivity for virtual ports and file, serial (to test), network streams, rtpmidi (work in progress).

*midipatcher* is ment to be platform-independent and is aimed at desktop systems (Linux, macOS, Windows) - being developed on macOS and occasionally tested on a RaspberryPi.

Can be run interactively, using a custom MIDI control port, or remotely using a (insecure, although password protected) TCP connection - the command syntax is text-based and generally identical, the MIDI control port uses a carrier SysEx format and splits messages if necessary into several max 128 byte long messages (as recommended by the specs). Due to the text-based format, it's easy to connect to on the console.

Provides virtual passthrough-ports (RawExec, MsgExec) to arbitrary executables that can server as any type of processors or filters communicating passing data to and fro STDIN, STDOUT respectively.

https://github.com/tschiemer/midipatcher

## Roadmap / Todos

- Test, bugfix and complete current source base
	- Introduce consistent error/exception system
- Add examples
	- Using Max/MSP and PureData matrices in combination with TCP control port
- Integrate RTPMIDI
- *Test* serial port - at the moment no test system.
- Add TLS for TCPControl..
- Add case insensitivity of portclasses (for creation descriptors) (?)
- Control command to save/load current setup (?)


## Guide to (Mostly) Harmless Command Line Interfacing

```bash
Usage:
	 midipatcher [-vh?]
	 midipatcher (--pc|--port-classes)
	 midipatcher [-d[-d[-d]]] (-l|-p||--ports)
	 		List all (found) ports.
			Number of found ports on single line followed by a line for each port consisting of <port-id> <port-type> <devstate> <port-descriptor>

	 midipatcher [-d[d[d]]] [-a<autoscan-interval>] [-u] [<control-type>] [<patch-options>]

	 	<control-type>
			(-i|--interactive) [--delimiter <delimiter>]
			(--tcpc|--tcp-control) [--tcp-pass <password>] [--delimiter <delimiter>]
			(--cp|--control-port) [--cp-in <control-in-port-descriptor>] [--cp-out <control-out-port-descriptor>]
			(-r|--remote) [--remote-in <remote-control-in-port-descriptor>] [--remote-out <remote-control-out-port-descriptor>] [--delimiter <delimiter>]

		<patch-options>
		  [-f|patch-file <patch-file>] [<in-port-descriptor1> <out-port-descriptor1> ... ]

Options:
	 -v 				 Show version.
	 -h|-? 				 Show this help.
	 -a|--autoscan <autoscan-interval>
					 Set regular rescanning of ports with given interval in millisec (default 1000; 0 = off).
	 -p|--ports 			 List known/detected ports (descriptors).
	 --pc|--port-classes 		 List registered port classes.
	 -d				Increase debug level by one.
	 -u				Show Updates.

	 -f|--patch-file <patch-file> 	 Use <patch-file> for patching configuration

	 -i|--interactive
					 Control from CLI

					 Commands:

					 ping
					    Connection test
					 version
					    Get midipatcher version
					 option <key> [<value>]
					    Get/set system/control option
					    <option-key>
					      return-ids
					          return port-id instead of port-key
					      notifications
					          enable/disable notifications
					      autoscan-enabled
					          enabled/disable autoscan
					      autoscan-interval
					          set autoscan interval
					 scan
					    Manual request for a device scan (if autoscan turned off).
					 portclasses
					    Get list of available port classes
					 ports [<port-id>|<port-key>]
					    Get basic port listing of given or all port(s); response contains in order: <port-id> <port-key> <type> <devstate> <port-desc>
					 devstate [<port-id>|<port-key>]
					    Get DeviceState of given or all port(s) (is device connected/enabled or not?)
					 portoption (<port-id>|<port-desc>) <key> [<value>]
					    Get set port option
					 register <port-descriptor>
					    Register new port using given descriptor
					 unregister (<port-id>|<port-key>)
					    Unregister port
					 constate [(<in-port-id>|<in-port-key>) [(<out-port-id>|<out-port-key>)]]
					      Get connection state between input- and output-port; either query specific connection, or list connections of specific/all input ports
					 connect (<in-port-id>|<in-port-key>) (<out-port-id>|<out-port-key>)
					    Connect two (registered) ports
					 disconnect (<in-port-id>|<in-port-key>) (<out-port-id>|<out-port-key>)
					    Disconnect two ports

	 --delimiter <delimiter>
	 				Use <delimiter> as (default) argument delimiter.

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

	 -r|--remote
	 --remote-in <remote-control-in-port-descriptor>
	 --remote-out <remote-control-out-port-descriptor>
					 Act as (interactive) remote control with default port options "--remote-in MidiOut:MidiPatcher-Control --remote-out MidiIn:MidiPatcher-Control".
					 Same commands as -i|--interactive

	 <port-id>
	 		(Process-internal) Unique ID of port.
	 <port-type>
	    1		Input
			2		Output
			3		Input/Output (Passthrough)
		<devstate>
			0		Device not connected
			1		Device connected

	 <port-descriptor> := <port-key>[<options>]
	 <port-key> := <PortClass>:<PortName>
	 <options> := (,<opt> ...)

Port classes, their basic descriptors and options:

	 MidiIn:<PortName>, 	 Connects (or waits for) MIDI port with given name
	 MidiOut:<PortName>
	 VirtMidiIn:<PortName> 	 Create virtual MIDI port with given name
	 VirtMidiOut:<PortName>
	 FileIn:(STDIN|<Filename>)		Opens said file/device/pipe for reading
	 		runningstatus=(0|1*)		  Use MIDI running status? (* = default)
	 FileOut:(STDOUT|STDERR|<Filename>)	Opens said file/device/pipe for writing
	 		runningstatus=(0*|1)			Use MIDI running status? (* = default)
	 UdpIn:[<listen-addr>:]<port>	Listen for UDP packets on given port (and interface)
	 		multicast=<multicast-addr>
			runningstatus=(0|1*)			Use MIDI running status? (* = default)
	 UdpOut:<remove-addr>:<port>	Send message as UDP packets to given addr:port
	 		runningstatus=(0*|1)			Use MIDI running status? (* = default)
	 StreamExec:(<exec>|<name>,exec=<exec>) Starts given exec and pass raw midi messages to STDIN and read again from STDOUT
	 		argv=<argv>
	 MsgExec:(<exec>|<name>,exec=<exec>) Starts given exec and pass parsed text-based MIDI message to STDIN and read again from STDOUT
	 		argv=<argv>

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
midipatcher --cp & midipatcher -r

More, fancy examples:
midimessage -g | midipatcher FileIn:STDIN RawExec:examples/RawExec/inc-channel RawExec:examples/RawExec/inc-channel FileOut:STDOUT | midimessage -p
midimessage -g | midipatcher FileIn:STDIN MsgExec:/bin/cat MsgExec:/bin/cat FileOut:STDOUT | midimessage -p
midimessage -g | midipatcher FileIn:STDIN MsgExec:my-inc,exec=examples/MsgExec/inc-cc-channel.sh MsgExec:my-inc FileOut:STDOUT | midimessage -p

Thanks to:

RtMidi: realtime MIDI i/o C++ classes, http://www.music.mcgill.ca/~gary/rtmidi
Asio (Networking) C++ Library, https://think-async.com/Asio
midimessage: midimessage library, https://github.com/tschiemer/midimessage

midipatcher-v0.1.0-72-g464cfff, MIT license, https://github.com/tschiemer/midipatcher
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





## Building

```bash
git clone --recursive https://github.com/tschiemer/midipatcher
cd midipatcher
cmake .
make

```

## Using as Library and Extending

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


For inspiration on how to use, have a look at `application/main.cpp` which shows different methods on how to populate the core `PortRegistry` and how to use `InjectorPort`s or the Logger, for that matter.

The `InjectorPort` also provides a rather convenient way of creating a passthrough (input-output)-port using a callback mechanism (see the remote control mechanism in `main.cpp`) without the need of creating any child-class.

To see how to use the text-based `midimessage`-syntax parser/generator see `MsgExec`.

## License

MIT License (also see `LICENSE` file).

Thanks to (and licensing according to component):

- [RtMidi: realtime MIDI i/o C++ classes](http://www.music.mcgill.ca/~gary/rtmidi)
- [Asio (Networking) C++ Library](https://think-async.com/Asio)
- [midimessage: a midimessage library](https://github.com/tschiemer/midimessage)
