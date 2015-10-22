#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>

#define NSTATES 8 // Don't need to include error state since once we trans, we never trans again. Use 8 as the error state
#define NINPUTS 7
#define OCT 8
#define INT 10
#define HEX 16

int currentState = 0;
int table[NSTATES][NINPUTS];

/* 
 * Function: 	get_next_state
 * ---------------------------
 * Transitions to the next state given the current state and the input.
 * 
 * returns: 0 	if it transitioned to the error state.
 *			1 	otherwise
 */
int get_next_state(int current_state, int inputType)
{
	currentState = table[current_state][inputType];
	printf("%d->%d\n", current_state, currentState);
	if (currentState == NSTATES) 
	{
		return 0; 
	}
	else return 1;
}

/*
 * Function:	get_input_type
 * ---------------------------
 * Gets the input type/category of the given character c.
 * 
 * Returns: 
 * 			When c is:
 * 		0   0-7 
 * 		1 	8,9
 * 		2 	b,B
 * 		3 	h,H
 * 		4 	a,A,c,C,d,D,e,E
 * 		5 	-,+
 * 		6 	error
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
		else return 6;
	} else if (c == '-' || c == '+') return 5;
	return 6;
}

/*
 * Function:	is_end_state
 * -------------------------
 * Checks if the current state is a valid end state for an input of the given base.
 *
 * Returns: 
 *		1 	if the current state is a valid end state
 * 		0 	otherwise
 */
int is_end_state(int base)
{
	return (((base == INT)&&(currentState == 1 || currentState == 3 || currentState == 7)) ||
			(base == HEX && currentState == 5) ||
			(base == OCT && currentState == 2));
}

/*
 * Function: 	print_lexical_token
 * --------------------------------
 * Takes a 32-bit octal, hexadecimal or signed integer constant and prints the corresponding lexical token.
 *
 * Params:
 *	base	The base that the constant is in
 *  lexeme	The string value of the constant 
 *
 */
void print_lexical_token(int base, char lexeme[])
{
	//TODO check overflow
	//Find decimal value
	int value = 0;

	if (base == INT)
	{
		int sign = 1;
		for (int i = 0; i < strlen(lexeme); i++)
		{
			if (lexeme[i] == '+') continue;
			if (lexeme[i] == '-')
			{
				sign = -1;
			}
			else {
				value *= 10;
				value += lexeme[i]-'0';
			}
		}
		value *= sign;
	} else {
		int power = 0;
		for (int i = strlen(lexeme)-2; i >= 0; i--) // Start from the least sig digit (before the oct/hex indicator)
		{
			if (isdigit(lexeme[i]))
			{
				value += (lexeme[i]-'0') * (pow(base, power));
			} else if (base == HEX && isalpha(lexeme[i]))
			{
				int currentDigit = tolower(lexeme[i]) - 'a';
				currentDigit += 10;
				value += currentDigit * (pow(base, power));
			}
			power++;
		}
	}
	printf("Number: %d\n", value);
}

int main(int argc, char *argv[]) {
	for (int i = 0; i < NSTATES; i++)
	{
		for (int j = 0; j < NINPUTS; j++)
		{
			table[i][j] = NSTATES; // Default transition is to error
		}
	}
	// table[X][Y] = Z; ....When in state X, on input Y, go to state Z
	table[0][0] = 1; table[0][1] = 3; table[0][2] = 4; 					table[0][4] = 4; table[0][5] = 6;
	table[1][0] = 1; table[1][1] = 3; table[1][2] = 2; table[1][3] = 5; table[1][4] = 4;
	table[2][0] = 4; table[2][1] = 4; table[2][2] = 4; table[2][3] = 5; table[2][4] = 4;
	table[3][0] = 3; table[3][1] = 3; table[3][2] = 4; table[3][3] = 5; table[3][4] = 4;
	table[4][0] = 4; table[4][1] = 4; table[4][2] = 4; table[4][3] = 5; table[4][4] = 4;

	table[6][0] = 7; table[6][1] = 7;
	table[7][0] = 7; table[7][1] = 7;


	for (int a = 1; a < argc; a++)
	{
		char* input = argv[a];
		int lengthOfInput = strlen(input);
		for (int i = 0; i < lengthOfInput; i++) { // for each char in the input
			int inType = get_input_type(input[i]);
			if (!get_next_state(currentState, inType))
			{
				break;
			}
		}
		if (!is_end_state(INT) && !is_end_state(HEX) && !is_end_state(OCT)) {
			currentState = NSTATES;
			printf("Error: Not a vaild hex, oct or int value.\n");
		} else { // it is a valid end state
			// calculate the decimal value & print it
			if (is_end_state(INT)) print_lexical_token(INT, input);
			else if (is_end_state(HEX)) print_lexical_token(HEX, input);
			else if (is_end_state(OCT)) print_lexical_token(OCT, input);
		}
		EXIT_SUCCESS;
	}
}