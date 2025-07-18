#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Uncomment this line to run calibration routine on every boot
// It needs Serial monitor to follow the steps, so it should only be used for development
// #define RUN_CALIBRATION_ON_BOOT

// Uncomment this line to enable Serial logging
// #define ENABLE_LOGGING

// Pin configuration
const uint8_t PIN_PITCH_BEND = A8;
const uint8_t PIN_MODULATION = A9;

// MIDI configuration
const uint8_t MIDI_CHANNEL_PRIMARY = 1;
const uint8_t MODULATION_CC_NUMBER = 1;

// Logic constants
const float SMOOTHING_ALPHA = 0.02;                 // Exponential smoothing factor for analog readings
const int PITCH_ACTIVE_THRESHOLD = 50;              // Helps prevent jitter
const int MOD_ACTIVE_THRESHOLD = 50;                // Helps prevent jitter
const unsigned long TIME_TO_REST_MS = 25;           // Time the wheel must be stable before it's considered at rest and snapped to digital center

/*
// Pitch bend calibration and logic
// TODO: these values should be determined by a calibration routine; for now they are hardcoded
const int PITCH_CAL_MIN = 1012;
const int PITCH_CAL_MAX = 3116;
const int PITCH_CAL_CENTER = 2168;

// Modulation wheel \calibration and logic
// TODO: these values should be determined by a calibration routine; for now they are hardcoded
const int MOD_CAL_MIN = 2080;
const int MOD_CAL_MAX = 3450;
*/


// ADC configuration
#define ADC_RESOLUTION 12                                   // 12-bit resolution (0-4095)
#define ADC_AVERAGING   16                                   // Number of samples to average for each reading
#define ADC_CONV_SPEED  ADC_CONVERSION_SPEED::HIGH_SPEED
#define ADC_SAMP_SPEED  ADC_SAMPLING_SPEED::HIGH_SPEED

#endif
