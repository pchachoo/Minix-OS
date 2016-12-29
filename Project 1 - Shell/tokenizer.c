#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "queuestruct.h"
#include "stackstruct.h"
#include "functions.h"

	//state = -1 for start/error
	//state = 0 for space
	//state = 1 for COMMA, newline
	//state = 2 for left paren
	//state = 3 for right paren
	//state = 4 for punctutaion
	//state = 5 for alphanumeric

struct queuenode * tokenizer (char *argv){
	int state = ERROR;
	int priority = 0;
	int revpriority = 255;
	char inputvalidity = true;//NEW to check if the sting is a valid command
	char * popparen = NULL;//NEW
	int bufferlen = strlen(argv);
	//char * fulltoken = NULL;
	char * token = (char *)malloc(bufferlen+1); //this is actually a command from shell input
	token[0] = '\0';
	int tokenlen = 0;

	for (int i = 0; i < bufferlen;){
		// spaces
		if(isspace(argv[i]))
		{
			if((state == COMMA) || (state == ERROR) || (state == SPACE))
				++i; //ignore the space at beginning of new command when tokenlen is 0
			else{
				tokenlen = strlen(token);
				token[tokenlen] = ' ';
				token[tokenlen+1] = '\0';
				++i;
			}
			state = SPACE;//set new state
		}

		//COMMA and new line as delimiters for new command
		else if((argv[i]==',')||(argv[i]=='\n'))
		{
			//add completed token to the queue
			revpriority = 255-priority;
			if (inputvalidity == true){//NEW copy only if input string is valid
				qenqueue(token, revpriority);
				inputvalidity = true; //reset
			}
			int n=i;
			while(argv[n-1]==')'){
				n--;
				priority--;
			}
			//if(state==RPAREN)
			//only works for single right paren, should be done for every right paren
			//empty token
			token[0] = '\0';
			//free(tempcommand); //TODO
			i++;
			state = COMMA;

		}

		// PARENTHESES
		else if ((argv[i]=='(')||(argv[i]==')'))
		{
			if(argv[i]=='('){
				if(!((state == COMMA )||(state == LPAREN )||(state == ERROR )||(state == SPACE )))//NEW deals with commands like b(c
					inputvalidity = false;
				spush(argv[i]);//push paren to the stack
				priority++;//increase priority of all commands till there is a matching right paren (pop)
				state = LPAREN; //left paren
			}
			else { //argv[i] = )
				if(!((state == ALNUM)||(state == SPACE)||(state == COMMA)||(state == RPAREN))) //NEW "(a; b;)c" or "(a; b);c" or "(a; b );c"
					inputvalidity = false;
				popparen = spop();
				if(popparen!='('){ //if its not a matched pair
	//				printf("Invalid symbol: Error in command. Please check the input.\n");
					printf("Error: Unmatched right parentheses.\n");
					inputvalidity = false;
					break;
				}
				else
					state = RPAREN; //right paren

			}
			++i; // we consumed this character
		}

		// any punctuation except ( ) ; and \n
		else if (ispunct(argv[i])) // returns a non-zero value(true) if c is a punctuation character else, zero (false)
		{
			//			printf("Input is punctuation");
			tokenlen = strlen(token);
			token[tokenlen] = argv[i];
			token[tokenlen+1] = '\0';
			i++;
			state = PUNCT;
		}

		// Alphanumeric
		else if (isalnum(argv[i]))
		{
			if(state==RPAREN){
				int n = i;
				while(argv[n-1]==')')
					n--;
				if(!(argv[n-1]==',')){
//					printf("Parenthesis between commands. Check input");
					inputvalidity = false;
					break;
				}
			} //if char just before this was a paren, then check if there was a comma right before the paren, else its invalid
			tokenlen = strlen(token);
			token[tokenlen] = argv[i];
			token[tokenlen+1] = '\0';
			i++;
			state = ALNUM;
		}
		else
		{ //do nothing
		}
		if((inputvalidity == false) || (state == ERROR)){//NEW start state should not remain ERROR after first iteration
				printf("Error in command. Please check the input.\n");
				break;
		}
	}//end of for
	popparen = spop();//pop from empty stack
	if((popparen!=NULL) || (state == ERROR))//if there are unmatched parentheses left
		printf("Error: Unmatched left parenthesis.\n");
	free(token);
	return firstelement();
}
