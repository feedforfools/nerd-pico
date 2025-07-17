#include <Arduino.h>

#include <MIDI.h>

#include "config/config.h"
#include "core/calibration.h"
#include "core/interfaces.h"
#include "hardware/analog_reader.h"
#include "io/midi/wheel_source.h"
#include "io/usb/usb_midi_sink.h"
#include "utils/logger.h"

AnalogReader analogReader;
WheelSource wheelSource;
UsbMidiSink usbMidiSink;

void setup()
{
	Logger::begin(9600);
	Logger::log("\n--- Nerd Pico initializing ---");

#if defined(RUN_CALIBRATION_ON_BOOT) && defined(HAVE_USB_SERIAL)
	Calibration::run();
#endif

	// Initialize components
	analogReader.init();
	wheelSource.init();
	Logger::log("Components initialized");

	// Establish connections
	analogReader.registerListener(&wheelSource);
	wheelSource.registerListener(&usbMidiSink);
	
	Logger::log("--- Initialization complete ---");
	Logger::flush();
}

void loop()
{
	analogReader.update();
}