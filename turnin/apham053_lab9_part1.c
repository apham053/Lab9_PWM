/*	Author: apham053
 *  Partner(s) Name: Steven Rodriguez
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency) {
  static double current_frequency;
  if (frequency != current_frequency) {
    if (!frequency) { TCCR3B &= 0x08; }
    else { TCCR3B |= 0x03; }
    if (frequency < 0.954) { OCR3A = 0xFFFF; }
    else if (frequency > 31250) { OCR3A = 0x0000; }
    else { OCR3A = (short) (8000000 / (128 * frequency)) - 1; }
    TCNT3 = 0;
    current_frequency = frequency;
  }
}

void PWM_on() {
  TCCR3A = (1 << COM3A0);
  TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
  set_PWM(0);
}

void PWM_off() {
  TCCR3A = 0x00;
  TCCR3B = 0x00;
}

#define C (~PINA & 0x01)
#define D (~PINA & 0x02)
#define E (~PINA & 0x04)

enum States { start, wait, noteC, noteD, noteE } State;

void tick() {

switch(State) {
    case start:
	State = wait;
	break;
    case wait:
	if (C) {
	    State = noteC;
	}
	else if (D) {
	    State = noteD;
	}
	else if (E) {
	    State = noteE;
	}
	else {
	    State = wait;
	}
	break;
    case noteC:
	if (C) {
	    State = noteC;
	}
	else {
	    State = wait;
	}
	break;
    case noteD:
	if (D) {
            State = noteD;
        }
        else {
            State = wait;
        }
	break;
    case noteE:
	if (E) {
            State = noteE;
        }
        else {
            State = wait;
        }
	break;
    default:
	State = start;
	break;
}

switch(State) {
    case start:
        break;
    case wait:
	set_PWM(0);
        break;
    case noteC:
	set_PWM(261.63);
        break;
    case noteD:
	set_PWM(293.66);
        break;
    case noteE:
	set_PWM(329.63);
        break;
    default:
	State = start;
	break;
}

} //tick

int main(void) {
    DDRA = 0x00;
    DDRB = 0xFF;
    PORTA = 0xFF;
    PORTB = 0x00;
    PWM_on();
    State = start;

    while (1) {
	tick();
    }

    return 1;
}
