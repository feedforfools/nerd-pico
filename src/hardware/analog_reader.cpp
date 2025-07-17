#include "analog_reader.h"

#include "config/config.h"
#include "core/calibration.h"
#include "utils/logger.h"

AnalogReader::AnalogReader() :
    adc(new ADC()),
    filteredPitchValue(calibrationData.pitchBend.center),
    lastSentPitchValue(0),
    isPitchAtRest(true),
    pitchLastMoveTime(0),
    filteredModValue(calibrationData.modulation.min),
    lastSentModValue(0),
    isModAtRest(true),
    modLastMoveTime(0)
{}

void AnalogReader::init()
{
    adc->adc0->setResolution(ADC_RESOLUTION);
    adc->adc0->setAveraging(ADC_AVERAGING);
    adc->adc0->setConversionSpeed(ADC_CONV_SPEED);
    adc->adc0->setSamplingSpeed(ADC_SAMP_SPEED);

    pinMode(PIN_PITCH_BEND, INPUT);
    pinMode(PIN_MODULATION, INPUT);

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
    filteredPitchValue = (SMOOTHING_ALPHA * rawValue) + ((1 - SMOOTHING_ALPHA) * filteredPitchValue);

    // Check if the wheel has been actively moved
    if (abs(filteredPitchValue - calibrationData.pitchBend.center) > PITCH_ACTIVE_THRESHOLD)
    {
        pitchLastMoveTime = millis();
        isPitchAtRest = false;

        // Constrain read values to prevent out-of-bounds errors
        int32_t constrainedValue = constrain(filteredPitchValue, calibrationData.pitchBend.min, calibrationData.pitchBend.max);
        // Map current raw value to 14-bit pitch bend range
        int16_t midiValue = map(constrainedValue, calibrationData.pitchBend.min, calibrationData.pitchBend.max, -8192, 8191);

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
            filteredPitchValue = calibrationData.pitchBend.center;
        }
    }
}

void AnalogReader::readModulation()
{
    int32_t rawValue = adc->adc0->analogRead(PIN_MODULATION);

    // Exponential smoothing filter
    filteredModValue = (SMOOTHING_ALPHA * rawValue) + ((1 - SMOOTHING_ALPHA) * filteredModValue);

    // Check if the modulation wheel has been actively moved
    if (abs(filteredModValue - calibrationData.modulation.min) > MOD_ACTIVE_THRESHOLD)
    {
        modLastMoveTime = millis();
        isModAtRest = false;

        // Constrain read values to prevent out-of-bounds errors
        int32_t constrainedValue = constrain(filteredModValue, calibrationData.modulation.min, calibrationData.modulation.max);
        // Map current raw value to 7-bit modulation range
        uint8_t midiValue = map(constrainedValue, calibrationData.modulation.min, calibrationData.modulation.max, 0, 127);

        if (midiValue != lastSentModValue)
        {
            if (listener) listener->onModulationChange(midiValue);
            lastSentModValue = midiValue;
        }
    }
    else
    {
        // Modulation wheel is near the minimum => check if it should be considered at rest
        if (!isModAtRest && (millis() - modLastMoveTime > TIME_TO_REST_MS))
        {
            isModAtRest = true;
            // Snap to digital minimum
            if (lastSentModValue != 0)
            {
                if (listener) listener->onModulationChange(0);
                lastSentModValue = 0;
            }
            filteredModValue = calibrationData.modulation.min;
        }
    }
}