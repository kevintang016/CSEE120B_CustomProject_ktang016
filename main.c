/*	Name: Kevin Tang (ktang016@ucr.edu)
 *	Lab Section: 024
 *	Assignment: Custom Project
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "i2c.h"
#define F_CPU 16000000UL

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
unsigned int check1, check2, check3, close, lock = 0;

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		// prevents OCR3A from underflowing, using prescaler 64
		// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		// set OCR3A based on desired frequency
		else { OCR3A = (short)(8000000 / (128 * frequency)) - 1; }
		TCNT3 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}
void PWM_on() {
	TCCR3A = (1 << COM3A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR3A
	TCCR3B = (1 << WGM32) | (1 << CS31) | (1 << CS30);
	// WGM32: When counter (TCNT3) matches OCR3A, reset counter
	// CS31 & CS30: Set a prescaler of 64
	set_PWM(0);
}
void PWM_off() {
	TCCR3A = 0x00;
	TCCR3B = 0x00;
}

enum BoxStates {Box_Start, Box_Locked1, Box_Locked2, Box_Open } BoxStates;
	
void Tick(){
	switch(Box_State){
		case Box_Start:
			Box_State = Box_Locked1;
			break;
		case Box_Locked1:
			if(check1 == 1){
				Box_State = Box_Locked2;
			}
			else{
				Box_State = Box_Locked1;
			}
			break;
		case Box_Locked2:
			if(check2 == 1 && check1 == 1){
				Box_State = Box_Open;
			}
			else if(check1 == 1 && check2 == 0){
				Box_State = Box_Locked1;
			}
			else{
				Box_State = Box_Locked2;
			}
			break;
		case Box_Open:
			if(close == 1 && check1 == 0 && check2 == 0){
				Box_State = Box_Locked1;
			}
			else if(close == 1 && check1 == 1 && check2 == 0){
				Box_State = Box_Locked2;
			}
			else if(check2 == 1 && check1 == 1){
				Box_State = Box_Open;
			}
			break;
	}
	
	switch(Box_State){
		case Box_Start:
			lock = 1;
			break;
		case Box_Locked1:
			get_locked1();
			break;
		case Box_Locked2:
			get_locked1();
			get_locked2();
			break;
		case Box_Open:
			close = 0;
			get_locked1();
			get_locked2();
			get_open();
			break;
	}
}

void get_locked1(){
	if(LightSensor()){
		check1 = 1;
	}
	else{
		check1 = 0;
	}
	return;
}

void get_locked2(){
	if(BluetoothSensor()){
		check2 = 1;
	}
	else{
		check2 = 0;
	}
	return;
}

void get_open(){
	if(MoveServo()){
		close = 1;
	}
	else{
		close = 0;
	}
	return;
}

int LightSensor(){
	if(MeasureLight > 200){
		return 1;
	}
	else{
		return 0;
	}
}

int BluetoothSensor(){
	if(MeasureBluetooth < 150){
		return 1;
	}
	else{
		return 0;
	}
}

int MoveServo(){
	if(close == 0){
		PORTA = PORTA | 0x01;
		PORTB = PORTB | 0x01;
		OpenServo();
		return 1;
	}
	else{
		PORTA = PORTA | 0x01;
		OpenServo();
		return 0;
	}
}

void OpenServo(){
	_delay_ms(3);
	return;
}

uint32_t MeasureBluetooth(){
	I2CSelectBluetooth();
	uint32_t r;
	
	_delay_ms(10);
	r = I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();

	return r;
}

uint32_t MeasureUV(){
	I2CSelectUV();
	uint32_t r;

	_delay_ms(10);
	r = I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();

	return r;
}

uint32_t MeasureIR(){
	I2CSelectIR();
	uint32_t r;

	_delay_ms(10);
	r = I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();

	return r;
}

uint32_t MeasureLight(){
	I2CSelectLight();
	uint32_t r;

	_delay_ms(10);
	r = I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();
	_delay_ms(10);
	r += I2CMeasure();

	return r;
}
	
uint32_t MeasureL(){
		
}

int main(void)
{
    const unsigned long timePeriod = 10;
	unsiged long SO_opentime = 3;
	
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	B = 0;
	TimerSet(timePeriod);
	TimerOn();
	Box_State = Box_Start;
	
    while (1){
		tempB = ~tempB;
		PORTB = tempB;
		Tick();
		while(!TimerFlag);
			TimerFlag = 0;
    }
}
