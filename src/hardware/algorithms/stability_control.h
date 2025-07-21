#ifndef STABILITY_CONTROL_H
#define STABILITY_CONTROL_H

#include <Arduino.h>

/**
 * @brief   Detects when a sensor value has "stalled" or "settled".
 *          This is useful for potentiometers that don't return perfectly
 *          to center, to avoid sending a constant, non-zero MIDI value.
 */
class StabilityControl
{
public:
    StabilityControl(uint16_t stabilityThreshold, uint16_t timeToSettle)
        : STABILITY_THRESHOLD(stabilityThreshold),
          TIME_TO_SETTLE_MS(timeToSettle),
          lastValue(0),
          stableStartTime(0),
          isPotentiallyStable(false)
    {
    }

    bool checkForStall(int32_t currentValue)
    {
        int32_t delta = abs(currentValue - lastValue);

        if (delta <= STABILITY_THRESHOLD)
        {
            // Value is stable, start or continue the timer
            if (!isPotentiallyStable)
            {
                isPotentiallyStable = true;
                stableStartTime = millis();
            }
        }
        else
        {
            // Value is moving, reset the stability check
            isPotentiallyStable = false;
        }

        lastValue = currentValue;

        // If it has been stable for long enough, declare it stalled
        if (isPotentiallyStable && (millis() - stableStartTime > TIME_TO_SETTLE_MS))
        {
            // We have stalled! Reset for the next movement
            isPotentiallyStable = false; 
            return true;
        }

        return false;
    }

    void reset(int32_t currentValue)
    {
        isPotentiallyStable = false;
        lastValue = currentValue;
    }

private:
    const uint16_t STABILITY_THRESHOLD; // Max change between readings to be considered "stable"
    const uint16_t TIME_TO_SETTLE_MS;   // How long it must be stable to be considered "settled"

    int32_t lastValue;
    unsigned long stableStartTime;
    bool isPotentiallyStable;
};

#endif