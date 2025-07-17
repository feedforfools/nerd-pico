#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <Arduino.h>

struct PitchBendCalibration
{
    // Default values for pitch bend calibration => overwritten by calibration routine
    int min = 1042;
    int max = 3076;
    int center = 2125;
};

struct ModulationCalibration
{
    // Default values for modulation calibration => overwritten by calibration routine
    int min = 2045;
    int max = 3455;
};

struct CalibrationData
{
    PitchBendCalibration pitchBend;
    ModulationCalibration modulation;
};

extern CalibrationData calibrationData;     // Global instance

namespace Calibration
{
    void run();
}

#endif