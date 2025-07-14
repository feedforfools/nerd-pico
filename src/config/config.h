#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

const uint8_t PIN_PITCH_BEND = A8;
const uint8_t PIN_MODULATION = A9;

// MIDI configuration
const uint8_t MIDI_CHANNEL_PRIMARY = 1;
const uint8_t MODULATION_CC_NUMBER = 1;

// Pitch bend calibration and logic
// TODO: these values should be determined by a calibration routine; for now they are hardcoded
const int PITCH_CAL_MIN = 1012;
const int PITCH_CAL_MAX = 3116;
const int PITCH_CAL_CENTER = 2168;

const int PITCH_ACTIVE_THRESHOLD = 5;                       // Helps prevent jitter
const unsigned long TIME_TO_REST_MS = 30;                   // Time the wheel must be stable before it's considered at rest and snapped to digital center

// Modulation wheel calibration and logic
// TODO: these values should be determined by a calibration routine; for now they are hardcoded
const int MOD_CAL_MIN = 516;
const int MOD_CAL_MAX = 863;

const int MOD_ACTIVE_THRESHOLD = 2;                         // Helps prevent jitter

// ADC configuration
#define ADC_RESOLUTION 12                                   // 12-bit resolution (0-4095)
#define ADC_AVERAGING   8                                   // Number of samples to average for each reading
#define ADC_CONV_SPEED  ADC_CONVERSION_SPEED::HIGH_SPEED
#define ADC_SAMP_SPEED  ADC_SAMPLING_SPEED::HIGH_SPEED

#endif
