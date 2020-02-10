/*	Author: apham053
 *  Partner(s) Name: Steven Rodriguez
 *	Lab Section:
 *	Assignment: Lab #9  Exercise #2
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

#define A0 (~PINA & 0x01)
#define A1 (~PINA & 0x02)
#define A2 (~PINA & 0x04)
int i;
double noteArr[8] = {261.63, 293.66, 329.63, 349.23, 392.00, 440.00, 493.88, 523.25};
enum States { start, off, on, up, down, play } State;

void tick() {

switch(State) {
    case start:
	State = off;
	break;
    case off:
	if (A0) {
	    State = on;
	}
	else {
	    State = off;
	}
	break;
    case on:
	State = play;
	break;
    case play:
	if (A0) {
	    State = off;
	}
	else if (A1) { 
	    State = up;
	}
	else if (A2) {
	    State = down;
	}
	else {
	    State = play;
	}
	break;
    case up:
	State = play;
	break;
    case down:
	State = play;
	break;
    default:
	State = start;
	break;
}

switch(State) {
    case start:
        break;
    case off:
	PWM_off();
        break;
    case on:
	PWM_on();
	i = 0;
	break;
    case play:
	set_PWM(noteArr[i]);
	break;
    case up:
	if (i < 7) {
	    i++;
	}
	break;
    case down:
	if (i > 0) {
            i--;
        }
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
