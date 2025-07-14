#include "usb_midi_sink.h"

#include "utils/logger.h"

UsbMidiSink::UsbMidiSink() {}

void UsbMidiSink::onMusicalEvent(const MusicalEvent& event)
{
    switch (event.type)
    {
        case MusicalEventType::PITCH_BEND:
            usbMIDI.sendPitchBend(event.value, event.channel);
            Logger::log("USB MIDI Pitch Bend: " + String(event.value) + " on channel " + String(event.channel));
            break;
        case MusicalEventType::CONTROL_CHANGE:
            usbMIDI.sendControlChange(event.data1, event.data2, event.channel);
            Logger::log("USB MIDI Control Change: CC " + String(event.data1) + " Value " + String(event.data2) + " on channel " + String(event.channel));
            break;
        default:
            Logger::log("Unknown MIDI event type received");
            break;
    }
}
