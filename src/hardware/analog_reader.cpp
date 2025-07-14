#include "analog_reader.h"

#include "config/config.h"
#include "utils/logger.h"

AnalogReader::AnalogReader() :
    adc(new ADC()),
    filteredPitchValue(PITCH_CAL_CENTER),
    lastSentPitchValue(0),
    isPitchAtRest(true),
    pitchLastMoveTime(0),
    filteredModValue(MOD_CAL_MIN),
    lastSentModValue(0)
{}

void AnalogReader::init()
{
    adc->adc0->setResolution(ADC_RESOLUTION);
    adc->adc0->setAveraging(ADC_AVERAGING);
    adc->adc0->setConversionSpeed(ADC_CONV_SPEED);
    adc->adc0->setSamplingSpeed(ADC_SAMP_SPEED);

    Logger::log("AnalogReader initialized");
}

void AnalogReader::registerListener(IAnalogControlListener* listener)
{
    this->listener = listener;
}

void AnalogReader::update()
{
    readPitchBend();
    readModulation();
}

void AnalogReader::readPitchBend()
{
    int32_t rawValue = adc->adc0->analogRead(PIN_PITCH_BEND);

    // Exponential smoothing filter
    filteredPitchValue = (0.1 * rawValue) + (0.9 * filteredPitchValue);

    // Check if the wheel has been actively moved
    if (abs(filteredPitchValue - PITCH_CAL_CENTER) > PITCH_ACTIVE_THRESHOLD)
    {
        pitchLastMoveTime = millis();
        if (isPitchAtRest)
        {
            isPitchAtRest = false;
        }

        // Map current raw value to 14-bit pitch bend range
        int16_t midiValue = map(filteredPitchValue, PITCH_CAL_MIN, PITCH_CAL_MAX, -8192, 8191);
        midiValue = constrain(midiValue, -8192, 8191);

        if (midiValue != lastSentPitchValue)
        {
            if (listener) listener->onPitchBendChange(midiValue);
            lastSentPitchValue = midiValue;
        }
    }
    else
    {
        // Wheel is near the center => check if it should be considered at rest
        if (!isPitchAtRest && (millis() - pitchLastMoveTime > TIME_TO_REST_MS))
        {
            isPitchAtRest = true;
            // Snap to digital center
            if (lastSentPitchValue != 0)
            {
                if (listener) listener->onPitchBendChange(0);
                lastSentPitchValue = 0;
            }
        }
    }
}

void AnalogReader::readModulation()
{
    int32_t rawValue = adc->adc0->analogRead(PIN_MODULATION);

    // Exponential smoothing filter
    filteredModValue = (0.2 * rawValue) + (0.8 * filteredModValue);

    // Map current raw value to 7-bit modulation range
    uint8_t midiValue = map(filteredModValue, MOD_CAL_MIN, MOD_CAL_MAX, 0, 127);
    midiValue = constrain(midiValue, 0, 127);

    if (abs(midiValue - lastSentModValue) > MOD_ACTIVE_THRESHOLD)
    {
        if (listener) listener->onModulationChange(midiValue);
        lastSentModValue = midiValue;
    }   
}