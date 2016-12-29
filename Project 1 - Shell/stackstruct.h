#include <stdio.h>
#define MAXSIZE 10

struct stack
{
    char * stackmem[MAXSIZE];
    int top;
};

void screate();
void spush(char paren);
char * spop(void);
void sdisplay(void);
void sdelete();
int ssize();
