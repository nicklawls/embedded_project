double frequencies[] = {261.63, 293.66, 329.63, 349.23, 392, 440, 493.88, 523.25};

// input: integer value corresponding to note
//	0 - 7 = c4 - c5
// output: frequency for the input note
double frequency(unsigned char note) { 
	
	if (note >= 0 && note <= 7) {	
		return frequencies[note];
	}
	
	else { // pwm will be silent
		return 0;
	}
}
