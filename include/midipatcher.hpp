#include "midipatcher/log.hpp"

#include "midipatcher/version.h"

#include "midipatcher/PortRegistry.hpp"
#include "midipatcher/PortDescriptor.hpp"

#include "midipatcher/Port/MidiIn.hpp"
#include "midipatcher/Port/MidiOut.hpp"
#include "midipatcher/Port/VirtMidiIn.hpp"
#include "midipatcher/Port/VirtMidiOut.hpp"
#include "midipatcher/Port/FileIn.hpp"
#include "midipatcher/Port/FileOut.hpp"
#include "midipatcher/Port/UdpIn.hpp"
#include "midipatcher/Port/UdpOut.hpp"
#include "midipatcher/Port/RawExec.hpp"
#include "midipatcher/Port/ControlPort.hpp"
#include "midipatcher/Port/InjectorPort.hpp"