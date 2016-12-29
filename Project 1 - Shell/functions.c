//
//  functions.c
//
//
//  Created by Thibault Timmerman on 14/09/2016.
//
//

#include "functions.h"
#include "queuestruct.h"

extern struct queuenode *first, *qcmd;


int nb_alias =0;
char *alias[512];
char *match[512];
char profile_path[128] = "/usr/shell/files/profile.src";
char alias_path[128] = "/usr/shell/files/alias.txt";


/* ############### INITIALIZATION FUNCTIONS ################## */
// Set up the shell
void shell_initialization(void)
{
    signal(SIGINT, ctrl_cSignal); // This sets the signal ctrl-c to quit the shell in case.
    
    // Read the profile file and copy the name of the variables in a buffer
    read_file();
    restaure_alias();

}


// Read the profile file
void read_file()
{
    FILE * file = fopen(profile_path, "r");
    
    if (file != NULL)
    {
        char env_variable[110]; // Buffer used to put the variable environment in.
        char env_path[512]; // Buffer used to put the path in.
        
        while(fgets(env_variable, sizeof(env_variable), file))
        {
            int j=0;
            int i=0;
            
            while(env_variable[i] != '=')
            {
                env_path[i] = env_variable[i];
                i++;
            }
            env_path[i]='\0';
            
            if(strcmp(env_path, "PATH") == 0)
            {
                i++;
                while(env_variable[i] != '\0' && env_path[i] != '\n'){
                    env_path[j] = env_variable[i];
                    j++;
                    i++;
                }
                env_path[j] = '\0';
                setenv("PATH", env_path, 1);
            }
            
            if(strcmp(env_path, "SIGN")==0){
                i++;
                while(env_variable[i] != '\0' && env_variable[i] != '\n'){
                    env_path[j] = env_variable[i];
                    j++;
                    i++;
                }
                env_path[j] = '\0';
                setenv("SIGN", env_path, 1);
            }
            
            if(strcmp(env_path, "HOME")==0){
                i++;
                while(env_variable[i] != '\0' && env_variable[i] != '\n')
                {
                    env_path[j] = env_variable[i];
                    j++;
                    i++;
                }
                env_path[j] = '\0';
                setenv("HOME", env_path, 1);
                chdir(env_path);
            }
        } //end while fgets
    } // end if
    else{
        char message[200];
        strcpy(message, strcat("Error opening profile file ", profile_path));
        printf("%s", message);
        setenv("HOME", "/root", 1);
        chdir("/root");
    }
    fclose(file);
}

//This function stores the alias in the memory
void restaure_alias(void)
{
    FILE * file;
    file = fopen(alias_path, "r"); //TODO
    
    char line[128];
    
    if(file == NULL)
    {
        perror("Error opening alias.txt file");
    }
    else
    {
        
        while(fgets(line, sizeof(line), file))
        {
            char *buffer_alias = calloc(128, sizeof(char));
            char *buffer_cmd = calloc(128, sizeof(char));
            int i = 0;
            int j = 0;
            
            while(line[i] != '=')
            {
                buffer_alias[i] = line[i];
                i++;
            }
            alias[nb_alias] = buffer_alias;
            i++;
            
            while(line[i] != '\n')
            {
                buffer_cmd[j] = line[i];
                i++;
                j++;
            }
            match[nb_alias] = buffer_cmd;
            
            nb_alias++;
            
            buffer_alias = NULL;
            buffer_cmd = NULL;
            free(buffer_cmd);
            free(buffer_alias);
            
        }
    }
    
    fclose(file);
    
}


// This function prompt a personal line before the command line
void prompt_my_line(void)
{
    char *sign = getenv("SIGN");
    
    getcwd(pathname, sizeof(pathname)/sizeof(char));
    printf("%s %s >", sign, pathname);
}



/* ################ READ COMMANDS FUNCTIONS ################## */

// This function reads the command line and puts it into an array
char read_cmd_line(char *command_line)
{
    char c;
    int i = 0; // Define the position of the cursor into the buffer
    while (((c = getchar())!='\n') && (i < SIZE_BUF - 1))// Checking for SIZE_BUF added
    {
        command_line[i] = c;
        i++;
    }

    command_line[i] = '\0';
    
    //int j=0;
    return *command_line;
    
}



/* ################ EXECUTION FUNCTIONS ###################### */

int is_alias(char *cmd)
{
    int i = 0;
    while (i < nb_alias)
    {
        if (strcmp(cmd, alias[i]) == 0)
        {
            strcpy(cmd, match[i]);
            return 1;
        }
        else
            i++;
    }
    
    return 0;
}


void qexecution()
{
    struct queuenode *qforexecution;
    
    if(first == NULL)
    {
        printf("The command entered is the empty command \n");
        return;
    }
    
    int cmdpricounter = 0;
    qforexecution = first;
    
    while(qforexecution!=NULL)
    {
        int status = 0;
        pid_t pid_child[MAX_CMD_NUM];
        int paral = 1;
        int no_process = 0;
        int eqprio = 1;
        
        while((qforexecution!=NULL) && (eqprio ==1) && (paral == 1))
        {
            char *args[50];
            int i = 0;
            int j = 0;
            int argument = 0;
            int temppri = qforexecution->priority;
            
            if (temppri == 255)
            {
                paral = 0;
            }
            
            while(qforexecution->command[i] != '\0')
            {
                char *buffer = calloc(128, sizeof(char));
                args[argument] = NULL;
                
                while(qforexecution->command[i] != ' ' && qforexecution->command[i] != '\0')
                {
                    buffer[j] = qforexecution->command[i];
                    i++;
                    j++;
                    args[argument] = buffer;
                }
                if(qforexecution->command[i] == ' ' && qforexecution->command[i+1] != '\0')
                {
                    argument++;
                    i++;
                    j=0;
                }
                buffer = NULL;
                free(buffer);
            } // End while to parse queue
            
            args[argument + 1] = NULL; // The last element of the array for execvp must be NULL
            
            if(strcmp(args[0], "alias") == 0)
            {
                create_alias(args[1]);
                return;
            } // End if alias
            
            is_alias(args[0]);
            
            if(strcmp(args[0], "exit") == 0)
            {
                save_alias();
                printf("Shell is closing, bye :-) \n");
                exit(0);
            } // end if exit
            
            if(strcmp(args[0], "cd") == 0)
            {
                if(args[1] == NULL)
                {
                    chdir(getenv("HOME"));
                    printf("Directory has been changed to /root");
                    return;
                }
                else
                {
                    chdir(args[1]);
                    return;
                }
            } // End if cd
            
            //Create a new process to execute the command.
            pid_child[no_process] = fork();
        
            if(pid_child[no_process] == -1)
            {
                perror("Error in fork");
            }
            else if(pid_child[no_process] == 0)
            {
                printf("fork : %s\n", args[0]);
                if(execvp(args[0], args) < 0)
                {
                    perror("Execvp error");
                    kill(getpid(), SIGTERM);
                    return;
                }
            }
            
            // Pointer now points to the following command
            qforexecution = qforexecution->ptr;
            
            // Compare priority between current command and following command
            if(qforexecution == NULL)
            {
                continue;
            }
            else
            {
                if(temppri == qforexecution->priority)
                {
                    eqprio = 1;
                }
                else
                {
                    eqprio = 0;
                }
            }
            
            // Update the number of processes executed.
            no_process++;
        } //end of while for commands of equal priority
        
        for(int nb_process = 0; nb_process < no_process + 1; nb_process++)
        {
            waitpid(pid_child[nb_process], &status, 0);
        }
        
        for(int a=0; a<cmdpricounter; a++)
        {
            qdequeue();//this should update 'first'
            //dequeue n times for n commands executed in parallel
        }
    } // End of while queue not empty
} // End of qexectution fonction


void create_alias(char *parameter)
{
    char tmp[512];
    
    strcpy(tmp, parameter);
    
    char *buffer_alias = calloc(128, sizeof(char));
    char *buffer_cmd = calloc(128, sizeof(char));
    int i = 0;
    int j = 0;
    
    
    
    while(tmp[i] != '=')
    {
        buffer_alias[i] = tmp[i];
        i++;
    }
    
    if(is_alias(buffer_alias) == 1)
    {
        printf("An alias with this name already exits \n");
        return;
    }
    
    alias[nb_alias] = buffer_alias;
    i++;
    
    while(tmp[i] != '\n')
    {
        buffer_cmd[j] = tmp[i];
        i++;
        j++;
    }
    match[nb_alias] = buffer_cmd;
        
    printf("%s\n", alias[nb_alias]);
    printf("%s\n", match[nb_alias]);

    nb_alias++;

    buffer_alias = NULL;
    buffer_cmd = NULL;
    free(buffer_cmd);
    free(buffer_alias);
    
    printf("Alias created");
    
}




/* ################ EXIT THE SHELL FUNTIONS ################## */

// This function will end the shell as asked, promprting a confirmation request
void ctrl_cSignal(int sig)
{
    char answer[20];
    printf("\n\t Do you really want to quit the shell? [Yes/No]");

    scanf("%15s", answer);

    if (strcasecmp(answer, "y")==0 || strcasecmp(answer, "yes") == 0)
    {
        save_alias();
        printf("Shell is closing, Good Bye :-) \n");
        exit(0);
    }
    else if(strcasecmp(answer, "n")==0 || strcasecmp(answer, "no") == 0)
    {
        
        printf("Press enter to continue\n");
        return;
    }
    else
    {
        return;
    }
}


void save_alias(void)
{
    FILE * file;
    file = fopen(alias_path, "w");
    int i =0;
    
    
    if(file == NULL)
    {
        perror("Error opening alias.txt file");
    }
    printf("Alias are saved in alias.txt file\n");
    
    while(i < nb_alias)
    {
        fprintf(file, "%s=%s\n", alias[i], match[i]);
        i++;
    }
    
    fclose(file);
}
