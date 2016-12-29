//
//  main.c
//
//
//  Created by Thibault Timmerman on 14/09/2016.
//
//

#include "functions.h"
#include "queuestruct.h"
#include "stackstruct.h"


int main(void)
{
    char command[SIZE_BUF];

    struct queuenode *qcmdlist;
    
    shell_initialization();
    
    while(1)
    {
        qcreate(); //set count = 0 and first = NULL
        screate(); // set top = -1
        
        // Print the characters required and found in the profile file
        prompt_my_line();
            
        // Read the command line
        read_cmd_line(command);
        
        int bufferlen = strlen(command);
        if((bufferlen!=0) && (command[bufferlen - 1] != ',')&& (command[bufferlen - 2] != ','))
        {
            command[bufferlen]= ',';
            command[bufferlen + 1] = '\0';
        }
        printf("command contains %s \n", command);
            
        // Make the queue
        qcmdlist = tokenizer(command);
        
        //Execute the commands
        qexecution();
        
        qdelete(qcmdlist);
        sdelete();
    }

    //Ends

    return 0;
}

