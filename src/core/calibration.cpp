#include "calibration.h"

#ifndef HAVE_USB_SERIAL

#include <ADC.h>
#include <Arduino.h>

#include "config/config.h"
#include "utils/logger.h"

CalibrationData calibrationData;

namespace Calibration
{
    int getStableReading(uint8_t pin)
    {
        long total = 0;
        const int readingsToAverage = 256;
        for (int i = 0; i < readingsToAverage; ++i)
        {
            total += analogRead(pin);
            delay(2); // Allow some time for the reading to stabilize
        }
        return total / readingsToAverage;
    }

    void waitForUserInput()
    {
        while (Logger::available() > 0)
        {
            Logger::read();
        }

        while (Logger::available() == 0)
        {
            delay(50);
        }
    }

    void run()
    {
        Logger::log("\n--- Starting Controller Calibration ---");
		Logger::log("Please follow the instructions carefully.");
		Logger::log("Press any key in the Serial Monitor to advance to the next step.");
		Logger::log("-----------------------------------------");
        ADC* adc = new ADC();
        adc->adc0->setResolution(ADC_RESOLUTION);
        adc->adc0->setAveraging(ADC_AVERAGING);
        adc->adc0->setConversionSpeed(ADC_CONV_SPEED);
        adc->adc0->setSamplingSpeed(ADC_SAMP_SPEED);

        pinMode(PIN_PITCH_BEND, INPUT);
        pinMode(PIN_MODULATION, INPUT);
		delay(2000);

		// 1. Pitch Bend Center
		Logger::log("\n[STEP 1/5] Release the joystick to its center position.");
		waitForUserInput();
		calibrationData.pitchBend.center = getStableReading(PIN_PITCH_BEND);
		Logger::log("--> Pitch Bend Center captured: " + String(calibrationData.pitchBend.center));
		delay(500);

		// 2. Pitch Bend Minimum (Left)
		Logger::log("\n[STEP 2/5] Move the joystick all the way to the LEFT.");
		waitForUserInput();
		calibrationData.pitchBend.min = getStableReading(PIN_PITCH_BEND);
		Logger::log("--> Pitch Bend Minimum captured: " + String(calibrationData.pitchBend.min));
		delay(500);

		// 3. Pitch Bend Maximum (Right)
		Logger::log("\n[STEP 3/5] Move the joystick all the way to the RIGHT.");
		waitForUserInput();
		calibrationData.pitchBend.max = getStableReading(PIN_PITCH_BEND);
		Logger::log("--> Pitch Bend Maximum captured: " + String(calibrationData.pitchBend.max));
		delay(500);

		// 4. Modulation Minimum (Rest)
		Logger::log("\n[STEP 4/5] Release the joystick to its modulation rest position.");
		waitForUserInput();
		calibrationData.modulation.min = getStableReading(PIN_MODULATION);
		Logger::log("--> Modulation Minimum captured: " + String(calibrationData.modulation.min));
		delay(500);

		// 5. Modulation Maximum
		Logger::log("\n[STEP 5/5] Move the joystick to its MAXIMUM modulation position.");
		waitForUserInput();
		calibrationData.modulation.max = getStableReading(PIN_MODULATION);
		Logger::log("--> Modulation Maximum captured: " + String(calibrationData.modulation.max));
		delay(500);

		Logger::log("\n--- Calibration Complete! ---");
		Logger::log("New values have been stored for this session.");
        Logger::log("Pitch Bend Center: " + String(calibrationData.pitchBend.center));
        Logger::log("Pitch Bend Min: " + String(calibrationData.pitchBend.min));
        Logger::log("Pitch Bend Max: " + String(calibrationData.pitchBend.max));
        Logger::log("Modulation Min: " + String(calibrationData.modulation.min));
        Logger::log("Modulation Max: " + String(calibrationData.modulation.max));
        Logger::log("You can now use the controller with the new calibration values.");
        Logger::log("Press any key to exit the calibration mode.");
		Logger::flush();
        waitForUserInput();
    }
}

#else

#include <Arduino.h>

CalibrationData calibrationData;

namespace Calibration
{
    void run() {}
}

#endif