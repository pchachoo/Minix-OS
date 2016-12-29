//
//  test.c
//  
//
//  Created by Thibault Timmerman on 24/10/2016.
//	Modified by Prachi Chachondia on 31/10/2016.
//


#include "mailbox.h"
#include <sys/types.h>
#include <stdio.h>
#include "pm.h"
#include <stdlib.h>
#include <minix/callnr.h>
#include <signal.h>
#include <sys/svrctl.h>
#include <sys/resource.h>
#include <sys/utsname.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/sysinfo.h>
#include <minix/vm.h>
#include <string.h>
#include <machine/archtypes.h>
#include <lib.h>
#include <assert.h>
#include "mproc.h"
#include "param.h"
#include <string.h>
#include <minix/type.h>
#include <minix/ipc.h> //for message struct

/*************************Creating Group of Receivers*****************************/
int make_group(int ** groups, int i)
{
    int numReceivers = 0;
    int j = 1;
    printf("How many receivers do you want to add?\n");
    scanf("%d", &numReceivers);
    groups[i] = malloc((numReceivers +1)*sizeof(int));
    groups[i][0] = numReceivers;
    
    if(numReceivers <= 0 || numReceivers > GROUPSIZE)
    {
        printf("Enter at most 64 receivers and at least 1 receiver\n");
        return -1;
    }
    while(j<numReceivers +1){
        int temp;
        printf("Enter the receiver ID %d\n", j);
        if(scanf("%d", &temp) == 0){
            printf("Enter a valid receive ID\n");
            return -1;
        }
        groups[i][j] = temp;
        j++;
    }
    return 1;
    
}

/*************************Removing Group of Receivers*****************************/
int remove_group(int ** groups, int i, int numGroups){
    int j = i;
    int numReceiversNextGroup;
    if(numGroups>0){
        while(j < numGroups-1){
            int k = 1;
            groups[i] = NULL;
            free(groups[i]);

            numReceiversNextGroup = groups[i+1][0];

            groups[i] = malloc((numReceiversNextGroup +1) *sizeof(int));
            groups[i][0] = numReceiversNextGroup;

            while(k< numReceiversNextGroup +1){
                groups[i][k] = groups[i+1][k];
                k++;
            }
            j++;
        }
    groups[j] = NULL;
    free(groups[j]);
    return 1;
    }
    else{
        printf("No group exists");
        return -1;
    }
}

/*************************Initializing Message*****************************/
void initMessage(message *mes)
{
    mes->m1_i1=0; //Message length
    mes->m1_i2=0;	//Number of receivers
    mes->m1_i3=0;
    mes->m1_p1=NULL;
    mes->m1_p2=NULL;
    mes->m1_p3=NULL;
    
}
//	m.m1_i1        	//Message length
//	m.m1_i2         //Number of receivers
//	m.m1_p1			//actual message
//	m.m1_p2		  	//list of receivers

/*************************Creating Mailbox for Receiver*****************************/
int test_createMailbox(int ownerID){//int recID
	printf("Creating Mailbox Test\n");
	message m;
	initMessage(&m);
  /*  if(ownerID<0){//invalid input
		 m.m1_i3=getpid();//get current process ID and send as owner to mailbox
	}
	else*/
	m.m1_i3=ownerID;//send ownerID to mailbox

	int x=_syscall(PM_PROC_NR,CREATEMAILBOX,&m);
	if(x == -1)
	{
		printf("Error\n");
	}
	else{
		printf("Success\n");
	}

	return x;
}

/*************************Remove Mailbox from Receiver*****************************/
int test_removeMailbox(int procID){
    printf("Removing mailbox from process Test %d\n",procID);
    message m;
    initMessage(&m);
    if(procID==-1){//remove mailbox for current process
    	m.m1_i3=getpid();
    }
    else
    	m.m1_i3=procID;
    printf("Removing mailbox from process Test %d\n",m.m1_i1);
    int x=_syscall(PM_PROC_NR,REMOVEMAILBOX,&m);
    
    if(x == -1)
    {
        printf("Error\n");
    }
    else{
        printf("Success\n");
    }
    return x;
}

/*************************Add Sender to Mailbox*****************************/
int test_addSender(int senderID, int ** groupslist){
    printf("Adding sender to mailbox Test \n");
    message m;
    initMessage(&m);
    m.m1_i1=senderID;
    char choice = 0;
    int tempID = 0;



    printf("Do you want to add the sender to current process [Enter 1] or to a group [Enter 2]?\n");
    scanf("%s", &choice);
    switch (choice){
        case '1':
        {
            m.m1_i3=getpid();
            int x=_syscall(PM_PROC_NR,ADDSENDER,&m);
            if(x == -1)
            {
                printf("Error\n");
            }
            else{
                printf("Success\n");
            }

            return x;
            break;
        }
        case '2':
        {
            printf("Enter id of group:\n");
            scanf("%d", &tempID);
            if(tempID<0 || tempID>=NUMGROUPRECEIVERS)
            {
                printf("Enter a valid group id. \n");
                return -1;
            }
            else{
                int numElements = groupslist[tempID][0];
                int j = 1;
                while(j<numElements +1)
                {
                    m.m1_i3 = groupslist[tempID][j];
                    //printf(" temp %d \t", m.m1_i3);
                    //printf(" group %d \n", groupslist[tempID][j]);
                    int x=_syscall(PM_PROC_NR,ADDSENDER,&m);

                    if(x == -1)
                    {
                    printf("Error\n");
                    }
                    else{
                    printf("Success\n");
                    }
                    j++;
                }
                return 1;
            }
            break;
        }
    }
}

/*************************Remove Sender from Mailbox*****************************/
int test_removeSender(int senderID){
    printf("Removing sender from mailbox of process Test \n");
    message m;
    initMessage(&m);
    m.m1_i1=senderID;
    m.m1_i3=getpid();//get current process ID and send as owner to mailbox
    int x=_syscall(PM_PROC_NR,REMOVESENDER,&m);
    
    if(x == -1)
    {
        printf("Error\n");
    }
    else{
        printf("Success\n");
    }
    
    return x;
    
}

/*************************Insert Message in Mailbox of Receiver*****************************/
int test_msend(char * mesAct, int * recList, int numRec, int mesLen){
    printf("Sending message to process Test \n");
    message m;
    initMessage(&m);
    m.m1_i1=mesLen;        	//Message length
    m.m1_i2=numRec;          //Number of receivers
    m.m1_i3=getpid();//get current process ID and send as owner to mailbox
    m.m1_p1=mesAct;			//actual message
    m.m1_p2=(char*)recList;  //list of receivers
    //printf("\nReady to call deposit... %d\n",(int)m.m3_p1);
    int x=_syscall(PM_PROC_NR,MSEND,&m);
    
    if(x == -1)
    {
        printf("Error\n");
    }
    else{
        printf("Success\n");
    }
    
    return x;
}

/*************************Remove Message from Mailbox of Sender*****************************/
int test_mreceive(char * senderAddr, int senderID){
    printf("Receiving message from process Test \n");
    message m;
    initMessage(&m);
    m.m1_i1=senderID;
    m.m1_i3=getpid();//get current process ID and send as owner to mailbox
    m.m1_p1=senderAddr;
    int x=-1;
    do{//receive all the pending messages from given sender in mailbox
        x=_syscall(PM_PROC_NR,MRECEIVE,&m);
        
        if(x == -1)
        {
            printf("Error\n");
            break;
        }
        else{
            printf("Success\n");
        }
        
    }while(x==-1);
    return x;
}

/* 6 Syscalls implemented:
 * int do_createMailbox()
 * int do_removeMailbox()
 * int do_addSender()
 * int do_removeSender()
 * int do_msend()
 * int do_mreceive()
 */

int main(void)
{
	int *groupslist[NUMGROUPRECEIVERS];
    int numGroups = 0;
    int numMes = 0, tempSender = 0, tempReceiver = 0, mesLen = 0, numRec = 0, numSend = 0;
    char option = 0;
	int counter = 0;
	int procID = -1, recID = 0, ownerID = 0;
	char *sentMessage = malloc(MESSAGESIZE);
	char *receivedMessage = malloc(MESSAGESIZE);
	int senderIDs[GROUPSIZE], receiverIDs[GROUPSIZE];
	for(int i=0;i<GROUPSIZE;i++){
		senderIDs[i]=0;
		receiverIDs[i]=0;
	}
    
    // int *groups = malloc(GROUPSIZE)
	//add current process as sender
	//senderIDs[0]=getpid();
	//numMes = 1;
	//tempSender = test_createMailbox(senderIDs,numMes);
	//test_addSender(tempSender);
	while(1) //infinite loop
	{
		printf("\n\n ##### Welcome to the test program for user process IPC in MINIX #####\n");
        printf("\t\t\t Current process ID = %d \n\n", getpid());
        
		printf("\t Choose your option \n");
		printf("\t 1. \t Create mailboxes for processes \n");
		printf("\t 2. \t Remove mailboxes for processes \n");
		printf("\t 3. \t Add a sender to the group for a process \n");
		printf("\t 4. \t Remove a sender from the group for a process \n");
		printf("\t 5. \t Send a message to a process \n");
		printf("\t 6. \t Receive a message from the mailbox \n");
        printf("\t 7. \t Add a group of receivers \n");
        printf("\t 8. \t Remove a group of receivers \n");
		printf("\t 9. \t Exit the test program\n\n");
		printf("\t Please enter option number: \t");
		scanf("%s", &option);

		switch(option)
		{
		case '1':
		{
			printf("Creating Mailbox for Given Process\n");
			int valid = 0;
			while(ownerID<0||valid==0){//if wrong ownerID or first iteration
				if(valid!=0)
					printf("Error in input. ");
				printf("Enter ID of receiver to create new mailbox for:\n");
				scanf("%d",&ownerID);
				valid++;
			}
		/*	while(numSend<=0 || numSend>GROUPSIZE){//force user to enter valid number of receivers from 1 to GROUPSIZE
				printf("Enter number of senders to add to new mailbox:\n");
				scanf("%d",&numSend);
			}
			counter = 0;//1 since we added current process as a sender before while
			while (counter<numSend) {
				printf("Enter id of sender:\n");
				scanf("%d",&senderIDs[counter]);
				counter++;
			}*/
			test_createMailbox(ownerID);//+1 since we added current process as a sender before while recID
			break;
		}
		case '2':
		{
			printf("Enter Process ID to remove Mailbox from, or enter -1 for Current Proccess");
			scanf("%d",&procID);
			printf("Removing Mailbox of current process\n");//current process cannot modify mailboxes of other processes
			test_removeMailbox(procID);
			break;
		}
		case '3':
		{
            printf("Enter Sender ID to add sender to approved list:\n");
            scanf("%d",&tempSender);
            test_addSender(tempSender, groupslist);
            break;
		}
		case '4':
		{
			printf("Enter Sender ID to remove sender:\n");
			scanf("%d",&tempSender);
			test_removeSender(tempSender);
			break;
		}
		case '5':
		{ //int test_msend(char * mesAct, int * recList, int numRec, int mesLen)
			numRec = 0;
            counter = 0;
            int groupID;
            char choice = 0;
            int choicevalid = 0;
			printf("Enter message to send (max 1024 characters):\n");
			getchar(); //clear newline from input before reading message
			fgets(sentMessage,MESSAGESIZE,stdin);
			mesLen = strlen(sentMessage);
            
            printf("Do you want to send a message to one process [enter 1] or to a group [enter 2]? \n");
            scanf("%s", &choice);
            
            switch (choice) {
                case '1':
                {
                    printf("Enter id of receiver:\n");
                    scanf("%d",&receiverIDs[counter]);
                    choicevalid = 0;
                    numRec = 1;
                    break;
                }
                case '2':
                {
                    printf("Enter id of the group:\n");
                    scanf("%d",&groupID);
                    numRec = groupslist[groupID][0];
                    while(counter < numRec +1)
                    {
                        receiverIDs[counter] = groupslist[groupID][counter+1];
                        counter ++;
                    }
                    choicevalid = 0;
                    break;
                }
                default:
                {
                    printf("Enter a valid choice\n");
                    break;
                }
            }
			if(choicevalid == 0)
            {
                test_msend(sentMessage,receiverIDs,numRec,mesLen);
            }
			break;
		}
		case '6':
		{
			printf("Enter Sender to Receive Message from:\n");
			scanf("%d",&tempReceiver);
			int recerror = test_mreceive (receivedMessage,tempReceiver);
			
            if(recerror!=-1)
				printf("Message received (max 1024 characters): %s\n",receivedMessage);
			else
				printf("No Message Received\n");
			break;
		}
        case '7':
        {
            if(numGroups < NUMGROUPRECEIVERS){
                if(make_group(groupslist, numGroups) == 1){
                    printf("Group created %d\n Success \n", numGroups);
                    numGroups++;
            }
                else{
                    remove_group(groupslist, numGroups+1, numGroups+1);
                }
            }
            else{
                printf("Too many groups created, maximum is 64\n");
            }
            break;
        }
        case '8':
        {
            int groupID;
            printf("Enter the ID of the group you want to remove:\n");
            scanf("%d", &groupID);
            if(remove_group(groupslist, groupID, numGroups) == 1){
                printf("Group %d removed", groupID);
                numGroups --;
            }
            break;
        }
		case '9':
		{
			printf("Have a nice day :-)\n");
			exit(0);
		}
		default: // Error case if the user enter an invalid command.
		{
			printf("Please enter a valid option\n");
			break;
		}
		} // End switch
	} // End while loop

	return 0;
} // End main

