#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Pin configuration
const uint8_t PIN_PITCH_BEND = A8;
const uint8_t PIN_MODULATION = A9;

// MIDI configuration
const uint8_t MIDI_CHANNEL_PRIMARY = 1;
const uint8_t MODULATION_CC_NUMBER = 1;

// Logic constants
const float SMOOTHING_ALPHA = 0.02;                     // Exponential smoothing factor for analog readings
const int MOD_ACTIVE_THRESHOLD = 50;                    // Helps prevent jitter
const unsigned long TIME_TO_REST_MS = 25;               // Time the wheel must be stable before it's considered at rest and snapped to digital center
const int PITCH_MIDI_SENSITIVITY = 75;                  // Minimum change in 14-bit value to send an update
const unsigned long MIN_PITCH_SEND_INTERVAL_MS = 10;    // Minimum time between sending pitch bend messages (10ms = 100Hz)

// ADC configuration
#define ADC_RESOLUTION 12                                   // 12-bit resolution (0-4095)
#define ADC_AVERAGING   16                                   // Number of samples to average for each reading
#define ADC_CONV_SPEED  ADC_CONVERSION_SPEED::HIGH_SPEED
#define ADC_SAMP_SPEED  ADC_SAMPLING_SPEED::HIGH_SPEED

#endif
