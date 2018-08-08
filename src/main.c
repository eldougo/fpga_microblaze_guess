/*
 * main.c
 *
 *  Created on: 7Aug.,2018
 *      Author: craig
 */


//AXI GPIO driver
#include "xgpio.h"

//send data over UART
#include "xil_printf.h"

//information about AXI peripherals
#include "xparameters.h"

//#include <unistd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <time.h>
#include "sleep.h"
//#include <inttypes.h>
//#include <string.h>
//#include <stdio.h>
#include <stdlib.h>

#define IN_BUF_SIZE 256
#define GUESS_RANGE 100

// Returned a line of text entered by the user
void get_uart_input(char *in_buffer){
	char8 in_char;
	size_t len = strlen(in_buffer);
	int cntr = 0;

	do {
		if (len < IN_BUF_SIZE-1){
			in_char = inbyte();
		} else {
			in_char = '\r';
		}

		// Enter pressed
		if (in_char == '\r'){
			xil_printf("\n\r");

		// backspace
		} else if (in_char == '\b' && len > 0) {
			len -= 1;
			in_buffer[len + 1] = '\0';
			xil_printf("\b \b");

		// Ignore non-printable chars
		} else if (in_char >= ' '){
			in_buffer[len] = in_char;
			in_buffer[len + 1] = '\0';
			len++;
			xil_printf("%c", in_char);

		// Echo non-printable chars
		} else {
			xil_printf("%c", in_char);
		}

		cntr++;

	} while (in_char != '\r');
}

// Check the string is all numeric
int is_str_numeric(char *in_buffer){
	int i;
	for(i = 0; i < strlen(in_buffer); i++){
		if (!isdigit(in_buffer[i])){
			return 0;
		}
	}
	return 1;
}

// Generate a random number when one of the outer buttons on the board are pressed.
int get_random_number(XGpio gpio){
	int rnd = 0;
	u32 btn;

	do {
		btn = XGpio_DiscreteRead(&gpio, 1);
		rnd = rnd + 1;
	} while (btn == 0);

	return rnd;
}

int guess(){
	XGpio gpio;
	char in_buffer[IN_BUF_SIZE] = "";
	int guess, secret_number, goes;

	XGpio_Initialize(&gpio, 0);

	XGpio_SetDataDirection(&gpio, 2, 0x00000000); // set LED GPIO channel tristates to All Output
	XGpio_SetDataDirection(&gpio, 1, 0xFFFFFFFF); // set BTN GPIO channel tristates to All Input

	for(goes = 0; goes < 6; goes++){
		xil_printf("\r\n\r\n\r\n\r\n\r\n\r\n\r\n\r\n");
	}
	xil_printf("  ####   #    #  ######   ####    ####\r\n");
	xil_printf(" #    #  #    #  #       #       #\r\n");
	xil_printf(" #       #    #  #####    ####    ####\r\n");
	xil_printf(" #  ###  #    #  #            #       #\r\n");
	xil_printf(" #    #  #    #  #       #    #  #    #\r\n");
	xil_printf("  ####    ####   ######   ####    ####\r\n");
	usleep(1000000);

	while (1)
	{
		goes = 0;
		guess = GUESS_RANGE + 1;
		xil_printf("\r\n\r\nThe World Famous Microblaze Guessing Game\r\n\r\n");
		usleep(1000000);
		xil_printf("Press one of the board's outer buttons to begin, or press the board's center button to reset.");
		secret_number = get_random_number(gpio) % (GUESS_RANGE + 1);
		strcpy(in_buffer, "");

		xil_printf("\r\n\r\nGuess the number between 0 to %d\r\n", GUESS_RANGE);

		do {

			goes++;

			// Get user input
			get_uart_input(in_buffer);

			if(is_str_numeric(in_buffer)){
				guess = atoi(in_buffer);

				if (guess > secret_number){
					xil_printf("Lower\r\n", goes);
				} else if(guess < secret_number){
					xil_printf("Higher\r\n", goes);
				} else {
					xil_printf("You got it in %d goes. Wanna play again? [Y|n]\r\n", goes);
					strcpy(in_buffer, "");
					get_uart_input(in_buffer);
					if (tolower(in_buffer[0]) == 'y'){
						xil_printf("\r\nCool! Lets play again.\r\n");
					} else {
						xil_printf("\r\nToo bad, we are playing again anyway :p \r\n");
						usleep(1000000);
					}
				}

			} else {
				xil_printf("%s isn't a number doofus! Try again\r\n", in_buffer);
			}

			strcpy(in_buffer, "");
		} while (guess != secret_number);

		usleep(1000000);
	}
}

int main(){
	return(guess());
}


