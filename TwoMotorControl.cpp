#include "Arduino.h"

// enumeration used to define the direction the motor
// is driven
enum MotorDirection {
	MotorDirectionForward = 1,
	MotorDirectionReverse = -1
};

// StepperMotor class is used to asynchronously
// drive a stepper motor.  The motor is driven using
// full-stepping where 2 coils are active during any
// one cycle.  
class Motor {
private:

	// the pin values represent which GPIO pin on
	// the microcontroller output the motor is attached to
	int pin1;
	int pin2;
	int pin3;
	int pin4;
	
	// the direction the motor is driving
	MotorDirection dir;

	// how long to wait in microseconds between state changes
	// on the output pins
	unsigned long cycleDuration;

	// which step the motor is currently on
	int step;

	// the last time the motor was stepped
	unsigned long lastStepTime;

public:
	Motor(int p1, int p2, int p3, int p4) :
		pin1(p1), pin2(p2), pin3(p3), pin4(p4),
		dir(MotorDirectionForward),
		cycleDuration(2000),
		step(0),
		lastStepTime(0)
	{
		// set all the pins on the motor to OUTPUT mode
		pinMode(pin1, OUTPUT);
		pinMode(pin2, OUTPUT);
		pinMode(pin3, OUTPUT);
		pinMode(pin4, OUTPUT);
	}

	void setDirection(MotorDirection d) {
		dir = d;
	}

	void setCycleDuration(unsigned long duration) {
		cycleDuration = duration;
	}

	// call tick frequently to update the motor state
	void tick();

private:

	// update the current step and set the pin voltages
	void doStep();

	// increment (or decrement) the step depending on the
	// direction and set the new step modulus 4
	void advanceStep();
};

void Motor::tick() {
	unsigned long now = micros();
	
	if (now - lastStepTime > cycleDuration) {
		doStep();
		lastStepTime = now;
	}
}

void Motor::advanceStep() {
	step = step + dir;

	if (step == 4)  step = 0;
	if (step == -1) step = 3;
}

void Motor::doStep() {
	advanceStep();

	switch (step) {
	case 0:  // 1010
		digitalWrite(pin1, HIGH);
		digitalWrite(pin2, LOW);
		digitalWrite(pin3, HIGH);
		digitalWrite(pin4, LOW);
		break;
	case 1:    // 0110
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, HIGH);
		digitalWrite(pin3, HIGH);
		digitalWrite(pin4, LOW);
		break;
	case 2:    //0101
		digitalWrite(pin1, LOW);
		digitalWrite(pin2, HIGH);
		digitalWrite(pin3, LOW);
		digitalWrite(pin4, HIGH);
		break;
	case 3:    //1001
		digitalWrite(pin1, HIGH);
		digitalWrite(pin2, LOW);
		digitalWrite(pin3, LOW);
		digitalWrite(pin4, HIGH);
		break;
	} 
}

Motor *m1 = NULL, *m2 = NULL;

void setup() {
	m1 = new Motor(4, 6, 5, 7);
	m1->setCycleDuration(2300);
	m2 = new Motor(8, 10, 9, 11);
	m2->setCycleDuration(4600);
}

void loop() {
	m1->tick();
    m2->tick();
	delayMicroseconds(10);
}

