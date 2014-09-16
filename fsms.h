/*      fsms.h - 6/5/14
 *      Nicolas Lawler <nlawl001@ucr.edu>:
 *      Lab Section: 23
 *      Assignment: Custom Lab 
 *      Exercise Description:
 *      
 *      I acknowledge all content contained herein, excluding template/example 
 *      code or unless otherwise notes, is my own original work
 */ 


/* Prepared State Machines and Notes */
/* Contains the nearly finished build upon SMS, SMS for control logic should exist in the project.c file*/
/* defines global flags and handshake variables */

#include "pwm.h"
#include "keypad.h"
#include "usart.h"
#include "matrix.h"
#include "seven_seg.h"
#include "rhythm.h"

const unsigned char kMaxSize = 32;
unsigned char rhythms[32];  
unsigned char notes[32];
unsigned char numNotes; 


void array_init() {
	for (int i = 0; i < kMaxSize; ++i) {
		rhythms[i] = 1;
		notes[i] = 3;
	}
}

void UpdateNumNotes() {
	for ( int i = 0; i < kMaxSize; ++i) {
		if (rhythms[i] == 0) {
			numNotes = i + 1;
			return;
		}
	}
	// array full size again
	numNotes = kMaxSize;
}

int TickFct_ToneChange(int state);
int TickFct_MapKey(int state);
int TickFct_Playback(int state);
int TickFct_Send(int state);
int TickFct_Record(int state);
int TickFct_MatrixController(int state);
int TickFct_7SegControler(int state);
int TickFct_RhythmChange(int state);

/*
 * SEND State machine
 * waits until button press, reads arrays, outputs to USART in about 4 seconds
 * #include "usart.h"
 * MUST Call initUSART() in main
 * goal period: 50ms
 */


enum SD_States { SD_WaitReqRaise, SD_SendMessage, SD_WaitReqLow } SD_State;
unsigned char send_req, send_ack;
int TickFct_Send(int state) {
	static unsigned char i;
	switch(state) { // Transitions
		case -1:
		i = 0;
		state = SD_WaitReqRaise;
		break;
		case SD_WaitReqRaise:
		if (!send_req) {
			state = SD_WaitReqRaise;
		}
		else if (send_req) {
			state = SD_SendMessage;
		}
		break;
		case SD_SendMessage:
		if (i < (numNotes-1)) {
			state = SD_SendMessage;
		}
		else if (!(i < (numNotes-1))) {
			state = SD_WaitReqLow;
			i = 0;
			//USART_Send('\n');
			USART_Flush();
			send_ack = 1;
		}
		break;
		case SD_WaitReqLow:
		if (send_req) {
			state = SD_WaitReqLow;
		}
		else if (!send_req) {
			state = SD_WaitReqRaise;
			send_ack = 0;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SD_WaitReqRaise:
		break;
		case SD_SendMessage:
		if (USART_IsSendReady()) {
			USART_Send(rhythms[i]);
			//USART_Send(' ');
			USART_Send(notes[i]);
			//USART_Send(',');
			i++;
		}
		
		break;
		case SD_WaitReqLow:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SD_State = state;
	return state;
}


/*
 * Tone Change State machine
 * Waits for flag raise, takes 1 tick to execute command 
 * #include "PWM.h"
 * currently takes input from C0 - C3(up, down, left, right) <- globals?
 * goal period: 100	or lower
 */

unsigned char change_flag, change_idle, up, down, left, right;
unsigned char frame_start, current_note;
enum TC_States { TC_WaitOk, TC_ChangeOk, TC_IncrementTone, TC_DecrementTone, TC_MoveLeft, TC_MoveRight } TC_State;
int TickFct_ToneChange(int state) {
	switch(state) { // Transitions
		case -1:
		state = TC_WaitOk;
		break;
		case TC_WaitOk:
		if (!change_flag) {
			state = TC_WaitOk;
		}
		else if (change_flag) {
			state = TC_ChangeOk;
			change_idle = 1;
		}
		break;
		case TC_ChangeOk:
		if (change_flag && !(up || down || left || right)) {
			state = TC_ChangeOk;
		}
		else if (change_flag && right && !(up || down || left)) {
			state = TC_MoveRight;
			change_idle = 0;
		}
		else if (change_flag && up && !(down || right || left)) {
			state = TC_IncrementTone;
			change_idle = 0;
		}
		else if (change_flag && left && !(up || down || right)) {
			state = TC_MoveLeft;
			change_idle = 0;
		}
		else if (change_flag && down && !(up || right || left)) {
			state = TC_DecrementTone;
			change_idle = 0;
		}
		else if (!change_flag) {
			state = TC_WaitOk;
			change_idle = 0;
		}
		break;
		case TC_IncrementTone:
		if (1) {
			state = TC_ChangeOk;
			PWM_off();
			change_idle =1;
		}
		break;
		case TC_DecrementTone:
		if (1) {
			state = TC_ChangeOk;
			PWM_off();
			change_idle = 1;
		}
		break;
		case TC_MoveLeft:
		if (1) {
			state = TC_ChangeOk;
			PWM_off();
			change_idle = 1;
		}
		break;
		case TC_MoveRight:
		if (1) {
			state = TC_ChangeOk;
			PWM_off();
			change_idle = 1;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case TC_WaitOk:
			current_note = 0;
			frame_start = 0;
		break;
		case TC_ChangeOk:
		break;
		case TC_IncrementTone:
		if (notes[current_note] < 7) {
			notes[current_note] += 1;
			PWM_on();
			set_PWM(frequency(notes[current_note]));
		}
		break;
		case TC_DecrementTone:
		if (notes[current_note] > 0) {
			notes[current_note] -= 1;
			PWM_on();
			set_PWM(frequency(notes[current_note]));
		}
		break;
		case TC_MoveLeft:
		if (current_note > 0) {
			current_note--;
			PWM_on();
			set_PWM(frequency(notes[current_note]));
		}
		
		if (current_note < frame_start ) {
			--frame_start;
		}
		break;
		case TC_MoveRight:
		if (current_note < (numNotes-1)) {
			current_note++;
			PWM_on();
			set_PWM(frequency(notes[current_note]));
		}
		
		if (current_note > (7 + frame_start)) {
			++frame_start;
		}
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	TC_State = state;
	return state;
}





/*
 * Playback State machine
 * Waits for request, plays back the song currently in memory
 * #include "PWM.h"
 * goal period: 100ish ms, tick rate dictates tempo, will be 2x period of Record 
 */


unsigned char playback_req, playback_ack;
enum PB_States { PB_WaitReqRaise, PB_PlayNote, PB_WaitReqLow } PB_State;
int TickFct_Playback(int state) {
	static unsigned char i, j;
	switch(state) { // Transitions
		case -1:
		i = 0;
		state = PB_WaitReqRaise;
		break;
		case PB_WaitReqRaise:
		if (!playback_req) {
			state = PB_WaitReqRaise;
		}
		else if (playback_req) {
			state = PB_PlayNote;
			i = 0;
			j = 0;
			PWM_on();
		}
		break;
		case PB_PlayNote:
		if (i < (numNotes-1)) {
			state = PB_PlayNote;
			//PWM_on();
			//if (frequency(notes[i]) == frequency(notes[i+1])) {
				
				//PWM_on(); // differentiate between held and consecutive
			//}
		}
		else if (!(i < (numNotes-1))) {
			state = PB_WaitReqLow;
			PWM_off();
			playback_ack = 1;
		}
		break;
		case PB_WaitReqLow:
		if (playback_req) {
			state = PB_WaitReqLow;
		}
		else if (!playback_req) {
			state = PB_WaitReqRaise;
			playback_ack = 0;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case PB_WaitReqRaise:
		break;
		case PB_PlayNote:
		
		set_PWM(frequency(notes[i]));
		if (j >(rhythms[i] - 2)) {
			j = 0;
			i++;
		} else {
			j++;
		}
		
		
		break;
		case PB_WaitReqLow:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	PB_State = state;
	return state;
}

/*
 * Keypad State machine
 * Continually Grabs and updates keypad
 * #include "keypad.h"
 * make sure to set port C correctly for keypad output
 * continue to update as more control features are added
 * goal period: 50ms 
 */
unsigned char playback, send, reset, rhythm_plus, rhythm_minus;
enum MK_States { MK_Read } MK_State;
int TickFct_MapKey(int state) {
	static unsigned char key;
	switch(state) { // Transitions
		case -1:
		state = MK_Read;
		break;
		case MK_Read:
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case MK_Read:
		key = GetKeypadKey();
		switch (key) {
			case '\0': //reset
				left = 0; right = 0;
				up = 0; down = 0;
				playback = 0;
				send = 0;
				reset = 0;
				rhythm_minus = 0;
				rhythm_plus = 0;
			break;
			case '1':
				rhythm_plus = 1;
			break;
			case '2':
				rhythm_minus = 1;
			break;
			case '3':
				send = 1;
			break;
			case '4':
				right = 1;
			break;
			case '6':
				playback = 1;
			break;
			case '7':
				up = 1;
			break;
			case '8':
				down = 1;
			break;
			
			case '9':
				reset = 1;
			break;
			case '*':
				left = 1;
			break;
			default:  break;
		}
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	MK_State = state;
	return state;
}



// Matrix Controller State Machine
// Continually Reads Notes array, starting from frame start, and prints it to console
// may add several different states for varying display configurations
// must #include "matrix.h" and call init_matrix()

enum MC_States { MC_UpdateMatrix } MC_State;
int TickFct_MatrixController(int state) {
	/*VARIABLES MUST BE DECLARED STATIC*/
	/*e.g., static int x = 0;*/
	/*Define user variables for this state machine here. No functions; make them global.*/
	switch(state) { // Transitions
		case -1:
		state = MC_UpdateMatrix;
		break;
		case MC_UpdateMatrix:
		if (1) {
			state = MC_UpdateMatrix;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case MC_UpdateMatrix:
		write_frame(frame_start, notes, current_note);
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	MC_State = state;
	return state;
}



/*
 * 7seg Controller SM
 * Continually writes current note length to 7Seg
 * set port a properly
 * goal period: 50ms 
 */
enum SevC_States { SevC_Update7Seg } SevC_State;
int TickFct_7SegControler(int state) {
	
	switch(state) { // Transitions
		case -1:
		state = SevC_Update7Seg;
		break;
		case SevC_Update7Seg:
		if (1) {
			state = SevC_Update7Seg;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case SevC_Update7Seg:
		Write7Seg(rhythms[current_note]);
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	SevC_State = state;
	return state;
}


// Rhythm Change SM
// Continually Reads Notes array, starting from frame start, and prints it to console
// may add several different states for varying display configurations
// must #include "matrix.h" and call init_matrix()
unsigned char rhythm_idle;
enum RC_States { RC_WaitOk, RC_ChangeOk, RC_DecrementWaitRelease, RC_IncrementWaitRelease } RC_State;
int TickFct_RhythmChange(int state) {
	
	switch(state) { // Transitions
		case -1:
		state = RC_WaitOk;
		break;
		case RC_WaitOk:
		if (change_flag) {
			state = RC_ChangeOk;
			rhythm_idle = 1;
		}
		else if (!change_flag) {
			state = RC_WaitOk;
		}
		break;
		case RC_ChangeOk:
		if (!change_flag) {
			state = RC_WaitOk;
			rhythm_idle = 0;
		}
		else if (change_flag && rhythm_minus) {
			state = RC_DecrementWaitRelease;
			rhythm_idle = 0;

			if (rhythms[current_note] > 0) {
				rhythms[current_note] -= 1;
				// numNotes check
				if (rhythms[current_note] == 0) {
					numNotes = current_note + 1;
					// might be off by one
					// dont want to get into the situation where you never get back!
				}
			}

		}
		else if (change_flag && !(rhythm_minus || rhythm_plus)) {
			state = RC_ChangeOk;
		}
		else if (change_flag && rhythm_plus) {
			state = RC_IncrementWaitRelease;
			rhythm_idle = 0;

			if (rhythms[current_note] < 8) {
				rhythms[current_note] += 1;
				if (rhythms[current_note] == 1) {
					UpdateNumNotes();
				}

			}
		}
		break;
		case RC_DecrementWaitRelease:
		if (rhythm_minus) {
			state = RC_DecrementWaitRelease;
		}
		else if (!rhythm_minus) {
			state = RC_ChangeOk;
			rhythm_idle = 1;
		}
		break;
		case RC_IncrementWaitRelease:
		if (rhythm_plus) {
			state = RC_IncrementWaitRelease;
		}
		else if (!rhythm_plus) {
			state = RC_ChangeOk;
			rhythm_idle = 1;
		}
		break;
		default:
		state = -1;
	} // Transitions

	switch(state) { // State actions
		case RC_WaitOk:
		break;
		case RC_ChangeOk:
		break;
		case RC_DecrementWaitRelease:
		break;
		case RC_IncrementWaitRelease:
		break;
		default: // ADD default behaviour below
		break;
	} // State actions
	RC_State = state;
	return state;
}

