#include "analog_reader.h"

#include <Arduino.h>

#include "config/config.h"
#include "core/calibration.h"
#include "utils/logger.h"

#define PITCH_STABILITY_THRESHOLD_ADC 4
#define PITCH_STALL_CHECK_ZONE_ADC 80
#define PITCH_TIME_TO_SETTLE_MS 150

AnalogReader::AnalogReader() :
    adc(new ADC()),
    pitchFilter(),
    pitchDeadzone(50, 100),
    pitchStability(PITCH_STABILITY_THRESHOLD_ADC, PITCH_TIME_TO_SETTLE_MS),
    lastSentPitchValue(0),
    lastPitchSendTime(0),
    filteredModValue(0.0f),
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

    // Reset filters to their calibrated initial states
    pitchFilter.reset(calibrationData.pitchBend.center);
    filteredModValue = calibrationData.modulation.min;
    pitchStability.reset(calibrationData.pitchBend.center);

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

    bool useStrongFiltering = !pitchDeadzone.isActive();
    uint32_t filteredValue = pitchFilter(rawValue, useStrongFiltering);

    if (pitchDeadzone.isActive())
    {
        int32_t distanceFromCenter = abs(int(filteredValue - calibrationData.pitchBend.center));
        if (distanceFromCenter < PITCH_STALL_CHECK_ZONE_ADC)
        {
            if (pitchStability.checkForStall(filteredValue))
            {
                pitchDeadzone.reset();
                if (lastSentPitchValue != 0)
                {
                    if (listener) listener->onPitchBendChange(0);
                    lastSentPitchValue = 0;
                }
                return;
            }
        }
        else
        {
            // If the value is outside the stall check zone, reset the stability control
            pitchStability.reset(filteredValue);
        }   
    }

    int16_t midiValue;
    bool shouldSendMidi = pitchDeadzone.processValue(filteredValue, calibrationData.pitchBend.center, midiValue,
                                                calibrationData.pitchBend.min, calibrationData.pitchBend.max);

    if (shouldSendMidi && midiValue != lastSentPitchValue)
    {
        unsigned long now = millis();
        int delta = abs(midiValue - lastSentPitchValue);

        // Send if the value change is significant OR enough time has passed since the last send
        if ((delta >= PITCH_MIDI_SENSITIVITY) || (now - lastPitchSendTime > MIN_PITCH_SEND_INTERVAL_MS))
        {
            if (listener) listener->onPitchBendChange(midiValue);
            lastSentPitchValue = midiValue;
            lastPitchSendTime = now;
            // Logger::log("Pitch: %d, Filt: %d, MIDI: %d, Active: %d", rawValue, filteredValue, midiValue, pitchDeadzone.isActive());
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