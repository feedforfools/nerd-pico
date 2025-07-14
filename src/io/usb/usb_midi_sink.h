#ifndef USB_MIDI_SINK_H
#define USB_MIDI_SINK_H

#include "core/interfaces.h"

class UsbMidiSink : public IMusicalEventSink
{
    public:
        UsbMidiSink();
        void onMusicalEvent(const MusicalEvent& event) override;
        PortId getPortId() const override { return 0; }
};

#endif