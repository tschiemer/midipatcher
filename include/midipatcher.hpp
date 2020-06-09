#include "midipatcher/version.hpp"

#include "midipatcher/Log.hpp"
#include "midipatcher/Error.hpp"

#include "midipatcher/PortRegistry.hpp"
#include "midipatcher/PortDescriptor.hpp"

#include "midipatcher/Patchfile.hpp"

#include "midipatcher/InteractiveControl.hpp"
#include "midipatcher/TCPControl.hpp"
#include "midipatcher/Port/ControlPort.hpp"
#include "midipatcher/Port/RemoteControlPort.hpp"

#include "midipatcher/Port/MidiIn.hpp"
#include "midipatcher/Port/MidiOut.hpp"
#include "midipatcher/Port/VirtMidiIn.hpp"
#include "midipatcher/Port/VirtMidiOut.hpp"
#include "midipatcher/Port/FileIn.hpp"
#include "midipatcher/Port/FileOut.hpp"
#include "midipatcher/Port/UdpIn.hpp"
#include "midipatcher/Port/UdpOut.hpp"
#include "midipatcher/Port/StreamExec.hpp"
#include "midipatcher/Port/MsgExec.hpp"
#include "midipatcher/Port/InjectorPort.hpp"
#include "midipatcher/Port/Serial.hpp"
