/*      matrix.h - 6/5/14
 *      Nicolas Lawler <nlawl001@ucr.edu>:
 *      Lab Section: 23
 *      Assignment: Custom Lab 
 *      Exercise Description:
 *      
 *      I acknowledge all content contained herein, excluding template/example 
 *      code or unless otherwise notes, is my own original work
 */ 

/* USAGE DISCLAIMER
 * Main driver code adapted from AVR Freaks user user david.prentice
 * Post URL: www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=78521
 */

#include <avr/io.h>

#if defined(AVR)
#ifndef DDR_WRITE
#define DDR_WRITE   0xE0      /* bits 0-2 o/p */
#define SDP8_CLOCK  (1<<7)
#define SDP8_LOAD   (1<<6)
#define SDP8_DATA   (1<<5)
#endif
#ifndef sdp8_data
#define sdp8_data       PORTD
#define sdp8_ctl        DDRD
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B
#define DECODE 0x09
#define SHUTDOWN 0x0C
#define DISPLAY_TEST 0x0F

#endif
#endif

void write_column(unsigned char, unsigned char);
void write_frame(unsigned char, unsigned char[], unsigned char current_note);

static void shift_8(unsigned char val)
{
	unsigned char cnt;
	sdp8_data &= (~SDP8_CLOCK & ~SDP8_LOAD);   /* CLOCK = LOAD = 0 */
	for (cnt = 8; cnt; --cnt, val <<= 1) {
		sdp8_data &= ~SDP8_DATA;
		if (val & 0x80) sdp8_data |= SDP8_DATA;
		sdp8_data |= SDP8_CLOCK;
		sdp8_data &= ~SDP8_CLOCK;
	}
	sdp8_data |= SDP8_DATA;
}

void send_command(unsigned char posn, unsigned char val)
{
	shift_8(posn);
	shift_8(val);
	sdp8_data |= SDP8_LOAD;
	sdp8_data &= ~SDP8_LOAD;
}

void init_matrix(void)
{
	sdp8_ctl = DDR_WRITE;
	send_command(INTENSITY, 0x01);   /* low intensity */
	send_command(SCAN_LIMIT, 0x07);   /* scan limit */
	send_command(DECODE, 0x00);   /* no decode */
	send_command(SHUTDOWN, 0x01);   /* not shutdown */
	send_command(DISPLAY_TEST, 0x00);   /* display test */
}

unsigned char row_mapping[] = {0x10, 0x80, 0x20, 0x40, 0x01, 0x04, 0x08, 0x02}; // flipped
//unsigned char notes[] = {2, 2, 2, 2, 2, 2, 2, 6, 8, 8, 4, 3, 3, 3, 6};

// writes a single cell specified by value to column specified by column
void write_column(unsigned char column, unsigned char value) {
	send_command(column, row_mapping[value]);
}

// writes an 8 element 'frame' of values in array values to display, starting at index start
// WARINING, DOES NOT CHECK IF START IS VALID INDEX, ENSURE START <= 8 - Size of array

void write_frame(unsigned char start, unsigned char values[], unsigned char current_note) {
	unsigned char j = 8;
	static unsigned char flash = 0;
	for (int i = start; (i < start+8); ++i) {
		if (i == current_note) { // current note flash
			if (flash > 1) { // flash every other tick
				send_command(j, 0); // write zero to column
				flash = 0;
			} else {
				write_column(j, values[i]);
				flash++;
			}
		} else {
			write_column(j, values[i]); // could be clearer but should work
		}
		j--;
	}
}