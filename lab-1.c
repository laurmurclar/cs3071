#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define NSTATES 10 // Don't need to include error state since once we trans, we never trans again. Use 8 as the error state
#define NINPUTS 7
#define OCT 8
#define INT 10
#define HEX 16

int zCount = 0; // keeps track of leading zeros
int currentState = 0;
char maxInt[] = "2147483647";
char minInt[] = "-2147483648";
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
	if (currentState == NSTATES) return 0; 
	else if (currentState == 1 || currentState == 2) zCount++;
	printf("Z: %d\n", zCount);
	return 1;
}

/*
 * Function:	get_input_type
 * ---------------------------
 * Gets the input type/category of the given character c.
 * 
 * Returns: 
 * 			When c is:
 *		0 	0
 * 		1   1-7 
 * 		2 	8,9
 * 		3 	b,B
 * 		4 	h,H
 * 		5 	a,A,c,C,d,D,e,E,f,F
 * 		6 	-,+
 * 		7 	error
 */	
int get_input_type(char c){
	if (isdigit(c)){
		if (c-'0' == 0) return 0;
		else if (c-'0' <= 7) return 1;
		else return 2;
	} else if (isalpha(c)) {
		if (c == 'b' || c == 'B') return 3;
		else if (c == 'h' || c == 'H') return 4;
		else if ((c >= 'a' && c <= 'f') || (c >= 'A' && c<= 'F')) return 5;
		else return 7;
	} else if (c == '-' || c == '+') return 6;
	return 7;
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
int is_end_state(int base){
	return (((base == INT) && (currentState == 1 || currentState == 2 || currentState == 3 || 
			currentState == 5 || currentState == 9)) || (base == HEX && currentState == 7) ||
			(base == OCT && currentState == 4));
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
 * Returns: 0 if overflow occurs
 *			1 otherwise
 */
int print_lexical_token(int base, char lexeme[]){
	// Check overflow
	printf("Base: %d\n", base);
	int nDigits = 0;
	
	if (base == OCT || base == HEX || (base == INT && (lexeme[0] == '-' || lexeme[0] == '+'))) {
		nDigits = strlen(lexeme) - zCount - 1; //-1 for the b,B,h or H
	} else {
		nDigits = strlen(lexeme) - zCount;
	}
	printf("nDigits: %d\n", nDigits);
	if ((base == OCT && nDigits > 11)||
		(base == HEX && nDigits > 8) ||
		(base == INT && nDigits > 10)){
		printf("Error, constant contains too many digits.\n");
		return 0;
	}
	else if (base == OCT && nDigits == 11 && lexeme[zCount]-'0' > 3) {
		printf("Error, constant cannot be represented as a 32-bit value.\n");
		return 0;
	}
	else if (base == INT && nDigits == 10){
		if (lexeme[0] == '-'){
			// Get the value without leading zeros
			char lex2[strlen(lexeme)-zCount];
			int seenZ = 0;
			for (int i = 0, j = 0; i < strlen(lexeme); i++){
				if (seenZ >= zCount || lexeme[i] != '0'){
					lex2[j] = lexeme[i];
					j++;
				}
				else seenZ++;
			}
			if (strcmp(lex2, minInt) > 0){
				printf("Error, constant cannot be represented as a 32-bit value.\n");
				return 0;
			}
		}
		else {
			//Get the value without leading zeros or the sign
			if (lexeme[0] == '+') zCount++;
			char lex2[strlen(lexeme)-zCount];
			strcpy(lex2, &(lexeme[zCount]));

			if (strcmp(lex2, maxInt) > 0){
				printf("Error, constant cannot be represented as a 32-bit value.\n");
				return 0;
			}
		}
	}
	// Find decimal value
	int value = 0;
	if (base == INT){
		int sign = 1;
		for (int i = 0; i < strlen(lexeme); i++){
			if (lexeme[i] == '+') continue;
			if (lexeme[i] == '-'){
				sign = -1;
			}
			else {
				value *= 10;
				value += lexeme[i]-'0';
			}
		}
		value *= sign;
	} else {
		int power = 1;
		for (int i = strlen(lexeme)-2; i >= 0; i--){ // Start from the least sig digit (before the oct/hex indicator)
			if (isdigit(lexeme[i])){
				value += (lexeme[i]-'0') * power;
			} else if (base == HEX && isalpha(lexeme[i])){
				int currentDigit = tolower(lexeme[i]) - 'a';
				currentDigit += 10;
				value += currentDigit * power;
			}
			power *= base;
		}
	}
	printf("Lexical token (constant, %d)\n", value);
	return 1;
}

int main(int argc, char *argv[]) {
	for (int i = 0; i < NSTATES; i++){
		for (int j = 0; j < NINPUTS; j++){
			table[i][j] = NSTATES; // Default transition is to error
		}
	}
	// table[X][Y] = Z; ....When in state X, on input Y, go to state Z
	table[0][0] = 1; table[0][1] = 3; table[0][2] = 5; table[0][3] = 6; 				 table[0][5] = 6; table[0][6] = 8;
	table[1][0] = 1; table[1][1] = 3; table[1][2] = 5; table[1][3] = 6; table[1][4] = 7; table[1][5] = 6;
	table[2][0] = 2; table[2][1] = 9; table[2][2] = 9;
	table[3][0] = 3; table[3][1] = 3; table[3][2] = 5; table[3][3] = 4; table[3][4] = 7; table[3][5] = 6;
	table[4][0] = 6; table[4][1] = 6; table[4][2] = 6; table[4][3] = 6; table[4][4] = 7; table[4][5] = 6;
	table[5][0] = 5; table[5][1] = 5; table[5][2] = 5; table[5][3] = 6; table[5][4] = 7; table[5][5] = 6;
	table[6][0] = 6; table[6][1] = 6; table[6][2] = 6; table[6][3] = 6; table[6][4] = 7; table[6][5] = 6;

	table[8][0] = 2; table[8][1] = 9; table[8][2] = 9;
	table[9][0] = 9; table[9][1] = 9; table[9][2] = 9;

	for (int a = 1; a < argc; a++){
		currentState = 0;
		zCount = 0;
		char* input = argv[a];
		int lengthOfInput = strlen(input);
		for (int i = 0; i < lengthOfInput; i++) { // for each char in the input
			int inType = get_input_type(input[i]);
			if (!get_next_state(currentState, inType)){
				break;
			}
		}
		if (!is_end_state(INT) && !is_end_state(HEX) && !is_end_state(OCT)) {
			currentState = NSTATES;
			printf("Error, constant is not a vaild Hexadecimal, Octal or Integer value.\n");
		} else { // it is a valid end state
			// calculate the decimal value & print it
			if (is_end_state(INT)) print_lexical_token(INT, input);
			else if (is_end_state(HEX)) print_lexical_token(HEX, input);
			else if (is_end_state(OCT)) print_lexical_token(OCT, input);
		}
		EXIT_SUCCESS;
	}
}