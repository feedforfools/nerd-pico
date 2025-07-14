#include "wheel_source.h"

#include "config/config.h"
#include "utils/logger.h"

WheelSource::WheelSource() {}

void WheelSource::init()
{
    Logger::log("WheelSource initialized");
}

void WheelSource::onPitchBendChange(int16_t value)
{
    MusicalEvent event;
    event.id = getPortId();
    event.type = MusicalEventType::PITCH_BEND;
    event.channel = MIDI_CHANNEL_PRIMARY;
    event.value = value;
    
    Logger::log("Pitch Bend Event: " + String(event.value));
    fireEvent(event);
}

void WheelSource::onModulationChange(uint8_t value)
{
    MusicalEvent event;
    event.id = getPortId();
    event.type = MusicalEventType::CONTROL_CHANGE;
    event.channel = MIDI_CHANNEL_PRIMARY;
    event.data1 = MODULATION_CC_NUMBER;
    event.data2 = value;

    Logger::log("Modulation Event: " + String(event.data2));
    fireEvent(event);
}