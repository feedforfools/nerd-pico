#include <Arduino.h>
#include <MIDI.h>
#include <EEPROM.h>
#include <ADC.h>

#define MIDI_CHANNEL 1
#define ALPHA 0.1
#define NOISE_THRESHOLD 5

MIDI_NAMESPACE::MidiInterface<MIDI_NAMESPACE::SerialMIDI<HardwareSerial>>* midiPort[1];
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);

ADC *ADC_CONTROLLER = new ADC();

struct calibrationRange
{
	int min;
	int max;
	int center;
};

const calibrationRange PITCH_BEND_CALIBRATION = {1012, 3116, 2168};
const calibrationRange MODULATION_CALIBRATION = {516, 863, 516};

const int DEAD_ZONE_RADIUS = 15;

calibrationRange calibratePitchBend();
calibrationRange calibrateModulation();
bool savePitchBendCalibration(calibrationRange range);
bool saveModulationCalibration(calibrationRange range);
calibrationRange loadPitchBendCalibration();
calibrationRange loadModulationCalibration();

void calibrate();
void play();
void readPitchBend();
int readModulation();

// State variables
int32_t filteredValue = PITCH_BEND_CALIBRATION.center;
int16_t lastSentValue = -1;

void setup()
{
	// Start serial comm for debug purposes
	Serial.begin(9600);

	midiPort[0] = &MIDI1;
	midiPort[0]->begin(MIDI_CHANNEL);

	ADC_CONTROLLER->adc0->setResolution(12);
	ADC_CONTROLLER->adc0->setAveraging(8);
	ADC_CONTROLLER->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED);
	ADC_CONTROLLER->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED);

	// calibrate();
}


void loop()
{
	readPitchBend();
}

void calibrate()
{
	Serial.println("Starting calibration...");
	delay(2000);

	calibrationRange pitchBendRange = calibratePitchBend();
	calibrationRange modulationRange = calibrateModulation();

	Serial.print("Pitch Bend Range: ");
	Serial.print(pitchBendRange.min);
	Serial.print(" - ");
	Serial.print(pitchBendRange.max);
	Serial.print(" (Center: ");
	Serial.print(pitchBendRange.center);
	Serial.println(")");
	Serial.print("Modulation Range: ");
	Serial.print(modulationRange.min);
	Serial.print(" - ");
	Serial.print(modulationRange.max);
	Serial.print(" (Center: ");
	Serial.print(modulationRange.center);
	Serial.println(")");

	Serial.println("Calibration complete.");
}

void play()
{
	int pitchBend = 0;
	int modulation = 0;
	int pastPitchBend = 0;
	int pastModulation = 0;
	pitchBend = analogRead(A8);
	modulation = analogRead(A9);

	if (pitchBend != pastPitchBend)
	{
	Serial.print("Pitch Bend: ");
	Serial.println(pitchBend);
	}
	if (modulation != pastModulation)
	{
		Serial.print("Modulation: ");
		Serial.println(modulation);
	}
	pastPitchBend = pitchBend;
	pastModulation = modulation;
	delay(100);
}

void readPitchBend()
{
	// Oversampling
	int32_t sum = 0;
	for (int i = 0; i < 16; i++) {
		sum += ADC_CONTROLLER->adc0->analogRead(A8);
		delayMicroseconds(100);
	}
	int32_t rawValue = sum >> 4;  // Divide by 16
	
	// Exponential filter
	filteredValue = (ALPHA * rawValue) + ((1.0 - ALPHA) * filteredValue);

	// Center snap with deadzone
	if (abs(filteredValue - PITCH_BEND_CALIBRATION.center) < DEAD_ZONE_RADIUS) {
		filteredValue = PITCH_BEND_CALIBRATION.center;
	}

	Serial.print("Filtered Pitch Bend Value: ");
	Serial.println(filteredValue);
	
	// Map to 14-bit MIDI range with calibration options
	int16_t midiValue = map(filteredValue, PITCH_BEND_CALIBRATION.min, PITCH_BEND_CALIBRATION.max, -8192, 8191);

	// Only send if change exceeds noise threshold
	if (abs(midiValue - lastSentValue) > NOISE_THRESHOLD) {
		Serial.print("Sending Pitch Bend: ");
		Serial.println(midiValue);

		// sendPitchBend(midiValue);
		lastSentValue = midiValue; 
	}
}

int readModulation()
{
	int modulation = ADC_CONTROLLER->adc0->analogRead(A9);
	modulation = map(modulation, MODULATION_CALIBRATION.min, MODULATION_CALIBRATION.max, -8192, 8191);
	return modulation;
}

calibrationRange calibratePitchBend()
{
	calibrationRange range;
	range.min = 0;
	range.max = 0;
	range.center = 0;

	// Calibrate the pitch bend range
	Serial.println("Calibrating Pitch Bend...");
	Serial.println("Do not touch the pitch bend wheel.");
	delay(5000);
	// Loop and continuously read the pitch bend value and average it
	Serial.println("Reading pitch bend center value...");
	for (int i = 0; i < 50; i++)
	{
		range.center += analogRead(A8);
		delay(100);
	}
	range.center /= 50;
	Serial.print("Center: ");
	Serial.println(range.center);

	Serial.println("Move the pitch bend wheel to its minimum position.");
	delay(5000);
	// Loop and continuously read the pitch bend value and average it
	Serial.println("Reading pitch bend min value...");
	for (int i = 0; i < 50; i++)
	{
		range.min += analogRead(A8);
		delay(100);
	}
	range.min /= 50;
	Serial.print("Min: ");
	Serial.println(range.min);

	Serial.println("Move the pitch bend wheel to its maximum position.");
	delay(5000);
	// Loop and continuously read the pitch bend value and average it
	Serial.println("Reading pitch bend max value...");
	for (int i = 0; i < 50; i++)
	{
		range.max += analogRead(A8);
		delay(100);
	}
	range.max /= 50;
	Serial.print("Max: ");
	Serial.println(range.max);

	return range;
}

calibrationRange calibrateModulation()
{
	calibrationRange range;
	range.min = 0;
	range.max = 0;
	range.center = 0;

	// Calibrate the modulation range
	Serial.println("Calibrating Modulation...");
	Serial.println("Do not touch the modulation wheel.");
	delay(5000);
	// Loop and continuously read the modulation value and average it
	Serial.println("Reading modulation center value...");
	for (int i = 0; i < 50; i++)
	{
		range.center += analogRead(A9);
		delay(100);
	}
	range.center /= 50;
	Serial.print("Center: ");
	Serial.println(range.center);

	Serial.println("Move the modulation wheel to its minimum position.");
	delay(5000);
	// Loop and continuously read the modulation value and average it
	Serial.println("Reading modulation min value...");
	for (int i = 0; i < 50; i++)
	{
		range.min += analogRead(A9);
		delay(100);
	}
	range.min /= 50;
	Serial.print("Min: ");
	Serial.println(range.min);

	Serial.println("Move the modulation wheel to its maximum position.");
	delay(5000);
	// Loop and continuously read the modulation value and average it
	Serial.println("Reading modulation max value...");
	for (int i = 0; i < 50; i++)
	{
		range.max += analogRead(A9);
		delay(100);
	}
	range.max /= 50;
	Serial.print("Max: ");
	Serial.println(range.max);

	return range;
}

bool savePitchBendCalibration(calibrationRange range)
{
	EEPROM.put(0, range);
	Serial.println("Pitch Bend calibration saved.");
	return true;
}

bool saveModulationCalibration(calibrationRange range)
{
	EEPROM.put(sizeof(calibrationRange), range);
	Serial.println("Modulation calibration saved.");
	return true;
}

calibrationRange loadPitchBendCalibration()
{
	calibrationRange range;
	EEPROM.get(0, range);
	Serial.println("Pitch Bend calibration loaded.");
	return range;
}

calibrationRange loadModulationCalibration()
{
	calibrationRange range;
	EEPROM.get(sizeof(calibrationRange), range);
	Serial.println("Modulation calibration loaded.");
	return range;
}