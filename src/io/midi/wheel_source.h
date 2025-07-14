#ifndef WHEEL_SOURCE_H
#define WHEEL_SOURCE_H

#include "core/interfaces.h"
#include "hardware/analog_reader.h"

class WheelSource : public IMusicalEventSource, public IAnalogControlListener
{
    public:
        WheelSource();
        void init();

        void onPitchBendChange(int16_t value) override;
        void onModulationChange(uint8_t value) override;
        PortId getPortId() const override { return PORT_ID_WHEEL_SOURCE; }
};

#endif