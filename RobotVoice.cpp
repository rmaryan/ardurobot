/*
 * RobotVoice.cpp
 *
 */

#include "RobotVoice.h"
#include <Arduino.h>

RobotVoice::RobotVoice(uint8_t in_voicePin) {
	voicePin = in_voicePin;
	pinMode(voicePin, OUTPUT);
	nextSound = sndNoSound;
}

RobotVoice::~RobotVoice() {

}

void RobotVoice::processTask() {
	switch(nextSound) {
	case sndNoSound: break;
	case sndHello:
		playTone(1600,100);
		playTone(2600,24);
		playTone(1900,92);
		delay(4);
		playFreqGlissando(2500, 3600, 1, 1900);
		playFreqGlissando(3600, 4200, 1, 1900);
		playFreqGlissando(4200, 3600, 1, 1900);
		playFreqGlissando(3600, 1900, 1, 1900);
		playFreqGlissando(550, 750, 1, 500);
		playFreqGlissando(750, 1200, 1, 500);
		playFreqGlissando(1200, 2000, 1, 500);
		playFreqGlissando(2000, 2100, 1, 300);
		delay(32);
		playTone(2350,36);
		playFreqGlissando(2350, 1600, 1, 500);
		break;
	case sndOK:
		playFreqGlissando(2300, 1900, 1, 1100);
		delay(28);
		playFreqGlissando(1800, 1700, 1, 1000);
		delay(36);
		playFreqGlissando(2000, 2200, 1, 1100);
		break;
	case sndQuestion:
		playTone(2400,40);
		playTone(1000,32);
		delay(28);
		playFreqGlissando(1700, 2400, 1, 1000);
		playTone(2400,32);
		delay(28);
		playFreqGlissando(2200, 1700, 1, 800);
		playTone(1700,24);
		delay(28);
		playFreqGlissando(1900, 2700, 1, 800);
		playTone(2700,21);
		delay(28);
		playFreqGlissando(2200, 1700, 1, 800);
		playTone(1700,24);
		delay(28);
		playFreqGlissando(1900, 2700, 1, 800);
		playTone(2700,21);
		break;
	case sndScared:
		playFreqGlissando(400, 550, 1, 14000);
		playFreqGlissando(550, 400, 1, 1200);
		playTone(2400,36);
		playFreqGlissando(1800, 2400, 1, 700);
		playTone(2400,36);
		delay(32);
		playFreqGlissando(2200, 1800, 1, 700);
		playFreqGlissando(1800, 1750, 1, 900);
		playTone(2350,36);
		playFreqGlissando(2350, 1200, 1, 700);
		delay(32);
		playFreqGlissando(700, 600, 1, 300);
		playFreqGlissando(600, 750, 1, 900);
	}
	nextSound = sndNoSound;
}

void RobotVoice::queueSound(VoiceSounds soundCode) {
	nextSound = soundCode;
}

/*
|| Beep with given freq, for given beats at given tempo
 */
void RobotVoice::playTone(unsigned int toneFrequency, byte beats) {
	int tone = (1000000 / toneFrequency)/2; //recalculate frequency to pause value between pulses
	for (long i = 0; i < beats * 1000L; i += tone * 2) {
		digitalWrite(voicePin, HIGH);
		delayMicroseconds(tone);
		digitalWrite(voicePin, LOW);
		delayMicroseconds(tone);
	}
}

void RobotVoice::playFreqGlissando( float freqFrom, float freqTo, float duty, float duration ) {

	duration=duration/8*3;

	int stepLength = 20; //30
	int i, numberOfSteps;
	float freqStep, freq;

	numberOfSteps  =  duration/stepLength;
	freqStep =  (freqTo - freqFrom)/numberOfSteps;

	freq = freqFrom;
	for ( i=0; i < numberOfSteps; i++ ) {
		playTone(freq, duty);
		freq = freq  + freqStep ;
	}
	playTone(freqTo, stepLength);
}
