#include <Arduino.h>
#include <MIDI.h>

#include "config/config.h"
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
	Serial.begin(9600);
	while (!Serial && millis() < 4000); // Wait for serial connection
	Logger::log("\n--- Nerd Pico initializing ---");

	// Initialize components
	analogReader.init();
	wheelSource.init();
	Logger::log("Components initialized");

	// Establish connections
	analogReader.registerListener(&wheelSource);
	wheelSource.registerListener(&usbMidiSink);
	
	Logger::log("--- Initialization complete ---");
	Serial.flush();
}

void loop()
{
	analogReader.update();
	usbMIDI.read();
}