#ifndef ANALOG_READER_H
#define ANALOG_READER_H

#include <Arduino.h>
#include <ADC.h>

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

        int32_t filteredPitchValue;
        int16_t lastSentPitchValue;
        bool isPitchAtRest;
        unsigned long pitchLastMoveTime;

        int32_t filteredModValue;
        uint8_t lastSentModValue;

        void readPitchBend();
        void readModulation();
};

#endif