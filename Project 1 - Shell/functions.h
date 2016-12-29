//
//  functions.h
//
//
//  Created by Thibault Timmerman on 14/09/2016.
//
//

#ifndef functions_h
#define functions_h

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

typedef int bool;
#define true 1
#define false 0
#define SIZE_BUF 512
#define SIGN_ENV_SIZE 4
#define HOME_ENV_SIZE 4
#define PATH_ENV_SIZE 4
#define MAX_CMD_NUM 32
#define MAX_CMD_SIZE 512
#define ERROR -1
#define SPACE 0
#define COMMA 1
#define LPAREN 2
#define RPAREN 3
#define PUNCT 4
#define ALNUM 5

char pathname[SIZE_BUF];

void shell_initialization(void);
char read_cmd_line(char *command_line);
void ctrl_cSignal(int sig);
void read_file();
void prompt_my_line(void);
void launch_command(void);
void save_alias(void);
void restaure_alias(void);
void create_alias(char *parameter);
void qexecution();
int is_alias(char *cmd);



#endif /* functions_h */
