#include <Arduino.h>
#include <Control_Surface.h>
#include <ResponsiveAnalogRead.h>

USBMIDI_Interface midi;
// USBDebugMIDI_Interface midi {115200};

const pin_t PITCH_BEND_PIN = A8;
const pin_t MODULATION_PIN = A9;

PBPotentiometer pitchBend { PITCH_BEND_PIN, Channel_1 };

CCPotentiometer modulationWheel { MODULATION_PIN, {MIDI_CC::Modulation_Wheel, Channel_1} };

constexpr analog_t maxRawValue = PBPotentiometer::getMaxRawValue();
constexpr analog_t minimumValue = 4075;
constexpr analog_t midValue = 8225;
constexpr analog_t maximumValue = 12375;
constexpr analog_t deadZoneRadius = 600;

bool dirty = false;

analog_t mappingFunction(analog_t raw)
{
	raw = constrain(raw, minimumValue, maximumValue);
	if (raw < (midValue - deadZoneRadius) || raw > (midValue + deadZoneRadius))
	{	
		dirty = true;
		// If outside the dead zone, map the value to the full range
		return map(raw, minimumValue, maximumValue, 0, maxRawValue);
	}
	return std::ceil(maxRawValue / 2); // If within the dead zone, return the mid value
}

// 4075 -  8500  - 12375
// 510 - 530 - 550

void setup()
{
	pitchBend.map(mappingFunction);
	Control_Surface.begin();
}

void loop()
{
	Control_Surface.loop();
	if (pitchBend.getValue()==512 && dirty)
	{ 
		Control_Surface.sendPitchBend(Channel_1 , (uint16_t) 8192); 
		dirty = false;
	}
	Serial.print(pitchBend.getRawValue());
	Serial.print("\t");
	Serial.print(pitchBend.getValue());
	Serial.println();
}