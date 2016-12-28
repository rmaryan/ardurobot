/*
 * RobotVoice.h
 *
 */

#ifndef ROBOTVOICE_H_
#define ROBOTVOICE_H_

#include "TaskInterface.h"

enum VoiceSounds {sndNoSound, sndHello, sndOK, sndQuestion, sndScared};

class RobotVoice: public TaskInterface {
private:
	uint8_t voicePin; // storage for the pin number where a beeper is connected
	VoiceSounds nextSound; // storage for the next sound in the queue
	void playTone(unsigned int toneFrequency, byte beats); // a function from the summer library
	void playFreqGlissando( float freqFrom, float freqTo, float duty, float duration); // a function from the summer library
public:
	/*
	 * Initializes the RobotVoice
	 * Stores the beeper pin to be used later
	 */
	RobotVoice(uint8_t in_voicePin);

	/*
	 * Destructor does pretty nothing
	 */
	~RobotVoice();

	/*
	 * This function is called by a task scheduler each time the world ticks
	 */
	virtual void processTask();

	/*
	 * Stores the ID of the next sound to be played right in the next time cycle
	 */
	void queueSound(VoiceSounds soundCode);
};

#endif /* ROBOTVOICE_H_ */
