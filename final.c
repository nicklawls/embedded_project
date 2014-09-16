/*      final.c - 6/5/14
 *      Nicolas Lawler <nlawl001@ucr.edu>:
 *      Lab Section: 23
 *      Assignment: Custom Lab 
 *      Exercise Description:
 *      
 *      I acknowledge all content contained herein, excluding template/example 
 *      code or unless otherwise notes, is my own original work
 */ 

#include <avr/io.h>
#include "task.h"
#include "timer.h"
#include "fsms.h"


int main()
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; // PWM on B6
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00; // LED Matrix

	
	// Declare tick functions
	int TickFct_Control(int state);

	// Period for the tasks
	unsigned long int SMTick1_calc = 200; // control
	unsigned long int SMTick2_calc = 100; // tone change
	unsigned long int SMTick3_calc = 50;  // keypad
	unsigned long int SMTick4_calc = 125; // playback
	unsigned long int SMTick5_calc = 100; // matrix
	unsigned long int SMTick6_calc = 50;  // send
	unsigned long int SMTick7_calc = 50;  // 7seg
	unsigned long int SMTick8_calc = 100; // rhythm change
 
	

	////Calculating GCD
	unsigned long int tmpGCD = 1;
	tmpGCD = findGCD(SMTick1_calc, SMTick2_calc);
	tmpGCD = findGCD(tmpGCD, SMTick3_calc);
	tmpGCD = findGCD(tmpGCD, SMTick4_calc);
	tmpGCD = findGCD(tmpGCD, SMTick5_calc);
	tmpGCD = findGCD(tmpGCD, SMTick6_calc);
	tmpGCD = findGCD(tmpGCD, SMTick7_calc);
	tmpGCD = findGCD(tmpGCD, SMTick8_calc);
	unsigned long int GCD = tmpGCD;

	//Recalculate GCD periods for scheduler
	unsigned long int SMTick1_period = SMTick1_calc/GCD;
	unsigned long int SMTick2_period = SMTick2_calc/GCD;
	unsigned long int SMTick3_period = SMTick3_calc/GCD;
	unsigned long int SMTick4_period = SMTick4_calc/GCD;
	unsigned long int SMTick5_period = SMTick5_calc/GCD;
	unsigned long int SMTick6_period = SMTick6_calc/GCD;
	unsigned long int SMTick7_period = SMTick7_calc/GCD;
	unsigned long int SMTick8_period = SMTick8_calc/GCD;
	// ...

	//Declare an array of tasks
	static task task1;
	static task task2;
	static task task3;
	static task task4;
	static task task5;
	static task task6;
	static task task7;
	static task task8;
	// ...
	task *tasks[] = { &task1, &task2, &task3, &task4, &task5, &task6, &task7, &task8}; // ...
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	// Task 1
	task1.state = -1;//Task initial state.
	task1.period = SMTick1_period;//Task Period.
	task1.elapsedTime = SMTick1_period;//Task current elapsed time.
	task1.TickFct = &TickFct_Control;//Function pointer for the tick.
	
	// Task 2
	task2.state = -1;
	task2.period = SMTick2_period;
	task2.elapsedTime = SMTick2_period;
	task2.TickFct = &TickFct_ToneChange;
	
	// Task 3
	task3.state = -1;
	task3.period = SMTick3_period;
	task3.elapsedTime = SMTick3_period;
	task3.TickFct = &TickFct_MapKey;
	
	// Task 4
	task4.state = -1;
	task4.period = SMTick4_period;
	task4.elapsedTime = SMTick4_period;
	task4.TickFct = &TickFct_Playback;
	
	// Task 5
	task5.state = -1;
	task5.period = SMTick5_period;
	task5.elapsedTime = SMTick5_period;
	task5.TickFct = &TickFct_MatrixController;
	
	// Task 6
	task6.state = -1;
	task6.period = SMTick6_period;
	task6.elapsedTime = SMTick6_period;
	task6.TickFct = &TickFct_Send;

	// Task 7
	task7.state = -1;
	task7.period = SMTick7_period;
	task7.elapsedTime = SMTick7_period;
	task7.TickFct = &TickFct_7SegControler;
	
	// Task 8
	task8.state = -1;
	task8.period = SMTick8_period;
	task8.elapsedTime = SMTick8_period;
	task8.TickFct = &TickFct_RhythmChange;
	// ...
	
	TimerSet(GCD);
	TimerOn();
	init_matrix();
	initUSART();
	

	unsigned short i; 
	while(1) {
	
		for ( i = 0; i < numTasks; i++ ) {
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		
		while(!TimerFlag);
		TimerFlag = 0;
	}

	// Error: Program should not exit!
	return 0;
}





enum C_States { C_Chilln, C_PlaybackWaitRelease, C_WaitPlaybackAck, C_WaitPlaybackAckLow, C_SendWaitRelease, C_WaitSendAck, C_WaitSendAckLow, C_Reset } C_State;
int TickFct_Control(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/

	switch(state) { // Transitions
		case -1:
		state = C_Reset;
		break;
		case C_Chilln:
		if (change_idle && rhythm_idle && playback) {
			state = C_PlaybackWaitRelease;
			change_flag = 0;
		}
		else if (change_idle && rhythm_idle && send) {
			state = C_SendWaitRelease;
			change_flag = 0;
		}
		else if (change_idle && rhythm_idle && reset) {
			state = C_Reset;
			change_flag = 0;
		}
		else {
			state = C_Chilln;
		}
		break;
		case C_PlaybackWaitRelease:
		if (playback) {
			state = C_PlaybackWaitRelease;
		}
		else if (!playback) {
			state = C_WaitPlaybackAck;
			playback_req = 1;
		}
		break;
		case C_WaitPlaybackAck:
		if (!playback_ack) {
			state = C_WaitPlaybackAck;
		}
		else if (playback_ack) {
			state = C_WaitPlaybackAckLow;
			playback_req = 0;
		}
		break;
		case C_WaitPlaybackAckLow:
		if (playback_ack || playback) {
			state = C_WaitPlaybackAckLow;
		}
		else if (!(playback_ack || playback)) {
			state = C_Chilln;
			change_flag = 1;
		}
		break;
		case C_SendWaitRelease:
		if (send) {
			state = C_SendWaitRelease;
		}
		else if (!send) {
			state = C_WaitSendAck;
			send_req = 1;
		}
		break;
		case C_WaitSendAck:
		if (!send_ack) {
			state = C_WaitSendAck;
		}
		else if (send_ack) {
			state = C_WaitSendAckLow;
			send_req = 0;
		}
		break;
		case C_WaitSendAckLow:
		if (send_ack || send) {
			state = C_WaitSendAckLow;
		}
		else if (!(send_ack || send)) {
			state = C_Chilln;
			change_flag = 1;
		}
		break;
		case C_Reset:
		if (1) {
			state = C_Chilln;
			change_flag = 1;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case C_Chilln:
		break;
		case C_PlaybackWaitRelease:
		break;
		case C_WaitPlaybackAck:
		break;
		case C_WaitPlaybackAckLow:
		break;
		case C_SendWaitRelease:
		break;
		case C_WaitSendAck:
		break;
		case C_WaitSendAckLow:
		break;
		case C_Reset:
		array_init();
		numNotes = kMaxSize;
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	C_State = state;
	return state;
}