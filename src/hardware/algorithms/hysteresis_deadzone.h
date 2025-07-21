#ifndef HYSTERESIS_DEADZONE_H
#define HYSTERESIS_DEADZONE_H

#include <Arduino.h>

class HysteresisDeadzone
{
public:
    HysteresisDeadzone(uint16_t innerDeadzone, uint16_t outerDeadzone) : 
        innerDeadzone(innerDeadzone), outerDeadzone(outerDeadzone), isInActiveState(false) {}

    bool processValue(int32_t filteredValue, int32_t centerValue, int16_t& midiValue, int32_t minValue, int32_t maxValue)
    {
        int32_t distanceFromCenter = abs(filteredValue - centerValue);

        if (!isInActiveState)
        {
            if (distanceFromCenter > outerDeadzone)
            {
                isInActiveState = true;
                midiValue = mapToBipolarMidi(filteredValue, centerValue, minValue, maxValue);
                return true;
            }
            // Stay at center => no MIDI update needed unless we need to send a 0 initially.
            // Caller can handle sending an initial 0 if needed.
            return false;
        }
        else // isInActiveState is true
        {
            if (distanceFromCenter <= innerDeadzone)
            {
                isInActiveState = false;
                midiValue = 0; // Snap to center
                return true;
            }

            // Still active => compute current MIDI value
            midiValue = mapToBipolarMidi(filteredValue, centerValue, minValue, maxValue);
            return true;
        }
    }

    bool isActive() const
    {
        return isInActiveState;
    }

    void reset()
    {
        isInActiveState = false;
    }

private:
    // A robust mapping for bipolar pitch bend values
    int16_t mapToBipolarMidi(int32_t value, int32_t center, int32_t min, int32_t max)
    {
        value = constrain(value, min, max);
        if (value >= center)
        {
            // Map [center, max] to [0, 8191]
            return map(value, center, max, 0, 8191);
        }
        else
        {
            // Map [min, center-1] to [-8192, -1]
            return map(value, min, center - 1, -8192, -1);
        }
    }

    uint16_t innerDeadzone;
    uint16_t outerDeadzone;
    bool isInActiveState;
};

#endif