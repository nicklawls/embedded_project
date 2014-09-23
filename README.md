# EE/CS 120B Custom Lab

## High Level Description
    
I designed and implemented a simple music maker that allows the user to compose a song. A song is created by specifying the duration and tone of up to 32 notes, and can be played back at any time while being written. Once ready, song data can be sent to a computer over a serial connection, and the music maker can be reset to create another song. 

## User Guide 
* The music maker initially begins with 32 notes, represented as single illuminated cells in columns of the LED matrix, each with a duration of 1. The current note flashes.
* To raise the and lower the pitch, hit 7 and 8 on the keypad. The note’s position on the matrix will raise and lower accordingly. Each note is in the major C scale. 
* Use * and 4 to move left and right between notes. As the user moves to notes not in the current frame, the display will shift accordingly (up to the maximum of 32 notes). 
* Buttons 1 and 2 increase and decrease the duration of the note, shown on the 7 segment display. Values range between 1 (a 16th note) and 8 (a half note). 
* To specify the end of the song, lower the duration of the note after the last note to 0. All notes after that note become inaccessible unless the duration is raised again. 
* At any time during song editing, the user can hit 6 to play the song, 3 to send song data over USART, and 9 to reset the duration and tone values. 

## Technologies/Components

### Hardware
* AVR AtMega1284 Microcontroller, Mini 8 by 8 LED Matrix, MAX7219 LED Matrix Driver IC, Silicon Labs USB to UART Bridge,Speaker, 7 Segment Display
### Software
* Atmel Studio 6, RIBS, Node.js/node-serialport

## Demo Video 
[Click here](http://youtu.be/jRDCazUHHsA) to view a video demonstration of the project.

[[youtube-{jRDCazUHHsA}-{688}x{387}]]

