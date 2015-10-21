#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

int currentState = 1;
/* Transitions to given state. Updates the current state. Advances the input
*/
int go_to_state(int state)
{

	currentState = state;
	// Perhaps doing some other stuff
	return 0;
}

/* function which given the input and the current state, 
tells you which state you need to transition to.
Note: Doesn't deal with the end marker. Thats dealt with in the function which calls this */
int get_next_state(int current_state, int inputType)
{
	switch (current_state){
		case 1:
			if (inputType == 0) { // 0-7
				go_to_state(2);
			} else if (inputType == 1) { // 8 or 9
				go_to_state(4);
			} else if (inputType == 2 || inputType == 4) { // b,B or aA,cC,dD,eE
				go_to_state(5);
			} else if (inputType ==5) { // -+
				go_to_state(7);
			} else {
				//ERROR
			}
			break;
		case 2:
			if (inputType == 0) { // 0-7
				go_to_state(2);
			} else if (inputType == 1) { // 8 or 9
				go_to_state(4);
			} else if (inputType == 2) { // b,B
				go_to_state(3);
			} else if (inputType == 3) { // h,H
				go_to_state(6);
			} else if (inputType == 4) { //  aA,cC,dD,eE
				go_to_state(5);
			}
			break;
		case 3:
			if (inputType == 0 || inputType == 1 || inputType == 2 || inputType == 4) { // 0-7, 8 or 9, b,B, aA,cC,dD,eE
				go_to_state(5);
			} else if (inputType == 3) { // h,H
				go_to_state(6);
			} else {
				//ERROR
			}
			break;
		case 4:
			if (inputType == 0 || inputType == 1) { // 0-7, 8 or 9
				go_to_state(4);
			} else if (inputType == 2 || inputType == 4) { // b,B, aA,cC,dD,eE
				go_to_state(5);
			} else if (inputType == 3) { // h,H
				go_to_state(6);
			} else {
				//ERROR
			}
			break;
		case 5:
			if (inputType == 0 || inputType == 1 || inputType == 2 || inputType == 4) { // 0-7, 8 or 9, b,B, aA,cC,dD,eE
				go_to_state(5);
			} else if (inputType == 3) { // h,H
				go_to_state(6);
			} else {
				//ERROR
			}
			break;
		case 6:
			// Error
			break;
		case 7:
		case 8:
			if (inputType == 0 || inputType == 1) { // 0-7, 8 or 9
				go_to_state(8);
			} else {
				//ERROR
			}
			break;
		default:
			//Error
			break;
	}
	return 0;
}

/* inputTypes 
* 0 = 0-7 
* 1 = 8|9
* 2	= b|B
* 3 = h|H
* 4 = aA,cC,dD,eE
* 5 = -+
* 6 = end/error/idk
*/	

/*
* Outputs the category of the given char.
*/	
int get_input_type(char c){
	if (isdigit(c))
	{
		if (c-'0' <= 7) return 0;
		else return 1;
	} else if (isalpha(c)) {
		if (c == 'b' || c == 'B') return 2;
		else if (c == 'h' || c == 'H') return 3;
		else if ((c >= 'a' && c <= 'e') || (c >= 'A' && c<= 'E')) return 4;
	} else if (c == '-' || c == '+') return 5;
	return 6;
}

// Checks if the current state is a valid end state for an integer input
int is_int_end_state()
{
	return (currentState == 2 || currentState == 4 || currentState == 8);
}

// Checks if the current state is a valid end state for a hexadecimal input
int is_hex_end_state()
{
	return (currentState == 6);
}

// Checks if the current state is a valid end state for an octal input
int is_oct_end_state()
{
	return (currentState == 3);
}

void print_decimal_of_int(char sval[])
{
	printf("Int: %s\n", sval);
}

void print_decimal_of_hex(char sval[])
{
	printf("Hex: %s\n", sval);
}

void print_decimal_of_oct(char sval[])
{
	printf("Oct: %s\n", sval);
}

int main() {
	char input[20];

	// get input
	printf("Enter a value: ");
	gets(input);
	int lengthOfInput = strlen(input);

	for (int i = 0; i < lengthOfInput; i++) { // for each char in the input
		// work out input type
		int inType = get_input_type(input[i]);
		// get next state
		get_next_state(currentState, inType);
	} // hit end marker
	if (!is_int_end_state() && !is_hex_end_state() && !is_oct_end_state()) { // not any of the valid end states
		// error, not a valid input
		printf("Error: Not a valid input.\n");
		EXIT_FAILURE;
	} else { // it is a valid end state
		// calculate the decimal value & print it
		if (is_int_end_state()) print_decimal_of_int(input);
		else if (is_hex_end_state()) print_decimal_of_hex(input);
		else if (is_oct_end_state()) print_decimal_of_oct(input);
	}
	EXIT_SUCCESS;
}