/*      parse.js - 6/5/14
 *      Nicolas Lawler <nlawl001@ucr.edu>:
 *      Lab Section: 23
 *      Assignment: Custom Lab 
 *      Exercise Description:
 *      
 *      I acknowledge all content contained herein, excluding template/example 
 *      code or unless otherwise notes, is my own original work
 *
 *		simple script to read from serial. Requires Node.js.
 *      Requires node module serialport (run: sudo npm install -g serialport)
 */ 


var SerialPort = require("serialport").SerialPort;
var serialport = new SerialPort("/dev/tty.SLAB_USBtoUART", {
	baudrate: 9600
});

var song = [];

serialport.on('open', function (){
	console.log('Serial Port Opened');
	var numEvents = 0;
	serialport.on('data', function(data) {	
		numEvents++;
		if (numEvents % 2) {
			console.log("rhythm: " + data[0]);
		} else {
			console.log("note: " + data[0]);
		}
	});	
});


