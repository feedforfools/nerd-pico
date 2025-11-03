#ifndef ANALOG_READER_H
#define ANALOG_READER_H

#include <Arduino.h>
#include <ADC.h>

#include "algorithms/adaptive_ema.h"
#include "algorithms/hysteresis_deadzone.h"
#include "algorithms/stability_control.h"

class IAnalogControlListener
{
    public:
        virtual ~IAnalogControlListener() = default;
        virtual void onPitchBendChange(int16_t value) = 0;
        virtual void onModulationChange(uint8_t value) = 0;
};

class AnalogReader
{
    public:
        AnalogReader();
        void init();
        void update();
        void registerListener(IAnalogControlListener* listener);

    private:
        ADC* adc;
        IAnalogControlListener* listener = nullptr;

        // Pitch bend filtering and control
        AdaptiveEMA<6, 3, uint32_t> pitchFilter;
        HysteresisDeadzone pitchDeadzone;
        StabilityControl pitchStability;
        int16_t lastSentPitchValue;
        unsigned long lastPitchSendTime;

        // Modulation wheel filtering and control
        float filteredModValue;
        uint8_t lastSentModValue;
        bool isModAtRest;
        unsigned long modLastMoveTime;
        unsigned long lastModSendTime;

        void readPitchBend();
        void readModulation();
};

#endif