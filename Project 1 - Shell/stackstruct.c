#include "stackstruct.h"
#include <stdio.h>
#include <stdlib.h>

struct stack s;

void screate(){
	s.top = -1;
/*	for(int i =0; i<MAXSIZE; i++){
		s.stackmem[i] = (char *)malloc(2);
		s.stackmem[i] = '\0';
	}*/
}

int ssize(){
	return s.top+1;//zero indexing
}

void sdelete(){
	for(int i=0; i<s.top; i++){
		s.stackmem[i] = '\0';
	}
	s.top = -1;
}

void spush(char paren){
	if(s.top == (MAXSIZE -1))
		printf("Error: Stack is full.\n");
	else {
		s.top = s.top+1;
		s.stackmem[s.top] = paren;
	}
}

char * spop(void){
	if(s.top == -1){//0 indexing
		//printf("Error: Stack is empty.\n");
		//Cannot print error message here as pop is also called to ensure stack is empty, which is not an error
		return NULL;
	}
	else
	{
		char * temp = s.stackmem[s.top];
		s.stackmem[s.top] = "";
		s.top = s.top-1;
		return temp;
	}
}

void sdisplay(void){
	for(int i=0;i<MAXSIZE;i++){
		if(s.stackmem[i]=='(')
			printf("\nStack row %d contains %s", i, s.stackmem[i]);
		if(s.stackmem[i]==')')
			printf("\nStack row %d contains )", i);
	}
}
