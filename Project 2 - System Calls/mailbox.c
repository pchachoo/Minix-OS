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

struct messageList *front=NULL, *rear=NULL, *temp;
struct mailBox *mailboxFront = NULL;
struct procDeadlock * deadlockOne = NULL; // Deadlock handling
//extern struct message;

/* 6 Syscalls implemented:
 * int do_createMailbox()
 * int do_removeMailbox()
 * int do_addSender()
 * int do_removeSender()
 * int do_msend()
 * int do_mreceive()
 */

/*************************Add Mailbox to Receiver*****************************/
int do_createMailbox()
{
	//printf("Creating Mailbox do_createMailbox\n");
	//add sender m_in.m1_i1 to who_e mailbox
	struct mailBox *cursor = mailboxFront;
	struct mailBox *mailboxToAdd;
	struct sender *senderToAdd;
	int mailboxexists = 0;
	int *senderList;
	int ownerID = m_in.m1_i3;//cast as int //size=m_in.m1_i1*sizeof(int) OR MESSAGESIZE?
	//sys_datacopy(who_e, (vir_bytes)m_in.m1_i1, SELF, (vir_bytes)ownerID, m_in.m1_i1*sizeof(int));
//	printf("who_e = %d, getpid = %d, recID[0] = %d\n",who_e, getpid(), ownerID);
	if(cursor!=NULL)
	{//if mailbox list exists
		cursor = mailboxFront;
		if(cursor->owner==ownerID){//mailbox already exists
			mailboxexists = 1;
			printf("Mailbox already exists.\n");
			return -1;
		}
		while(cursor->next!=NULL){//check entire mailbox list to make sure mailbox does not exist
			cursor = cursor->next;
			if(cursor->owner==ownerID){//mailbox already exists
				mailboxexists = 1;
				printf("Mailbox already exists.\n");
				return -1;
			}
		}//end of while
		//cursor.next now points to the end of the list of mailboxes and is NULL
		if(mailboxexists==0) //mailbox not found in the list
		{//need to create a mailbox
			printf("Creating new mailbox to add to list.\n");
			mailboxToAdd = (struct mailBox *)malloc(sizeof(struct mailBox));
			mailboxToAdd->numMes = 0;
			mailboxToAdd->owner = ownerID;
			mailboxToAdd->mbList = NULL;
			mailboxToAdd->mbSender = NULL;
			mailboxToAdd->mbReceiver = NULL;
			mailboxToAdd->next = NULL;
			cursor->next=mailboxToAdd;
			return 1; //return success
		}
	}
	else{//cursor==NULL hence mailbox list does not exist
		printf("Creating new mailbox and creating new list.\n");
		mailboxToAdd = (struct mailBox *)malloc(sizeof(struct mailBox));
		mailboxToAdd->numMes = 0;
		mailboxToAdd->owner = ownerID;
		mailboxToAdd->mbList = NULL;
		mailboxToAdd->mbSender = NULL;
		mailboxToAdd->mbReceiver = NULL;
		mailboxToAdd->next = NULL;
		mailboxFront=mailboxToAdd;//new mailboxlist head pointer
		cursor = mailboxFront;
		return 1;
	}
	//if control comes here, there was an error
	printf("Mailbox could not be created.\n");//should never happen
	return -1;//end of mailboxexists if

}

/*************************Remove Mailbox from Receiver*****************************/
int do_removeMailbox()//1 = mailbox removed successfully, -1=error
{
//	printf("Removing Mailbox do_removeMailbox on process %d\n",m_in.m1_i1);
	struct mailBox *cursor = mailboxFront, *mailboxToRemove, *temp;
	if(cursor==NULL){//no mailbox to delete
		printf("No mailbox to remove\n");
		return -1;
	}
	if(cursor->owner==m_in.m1_i3)//deal with first element of mailbox list separately
	{//WORKS!!!
		printf("Removing Mailbox %d.\n", cursor->owner);
		mailboxToRemove = cursor;
		cursor = cursor->next;
		mailboxFront = cursor;
		free(mailboxToRemove->mbList);
		free(mailboxToRemove->mbSender);
		free(mailboxToRemove->mbReceiver);
		free(mailboxToRemove);
		mailboxToRemove = NULL;
		return 1;
	}
	else{
//		printf("running loop till we find the correct mailbox\n");
		int found = 0;
		struct mailBox * prev;
		temp = cursor;
		while(temp->next!=NULL){
			if(temp->next->owner == m_in.m1_i3){
				printf("Removing Mailbox %d.\n", temp->next->owner);
				prev = temp;
				found = 1;
				break;
			}
			temp=temp->next;
		}
		if(found==1){
			prev = temp->next;
			temp->next = prev->next;
			free(prev->mbList);
			free(prev->mbSender);
			free(prev->mbReceiver);
			free(prev);
			prev=NULL;
			return 1;
		}
		if(found==0){
			printf("Mailbox not found.\n");
			return -1;
		}
	}
}//end of removeMailbox


/*************************Add Sender to Mailbox*****************************/
int do_addSender()
{
//	printf("Adding Sender do_addSender\n");
	//add sender m_in.m1_i1 to who_e mailbox
	struct mailBox *cursor = mailboxFront;
	struct sender *senderToAdd, *mbSenderFront;
	int ownerID=m_in.m1_i3;
	if(cursor!=NULL)
	{//if mailbox exists
		while(cursor!=NULL){
			if(cursor->owner==ownerID){
				mbSenderFront = cursor->mbSender;
				//	senderToAdd=cursor->mbSender;
				if(cursor->mbSender ==NULL){//no senders exist yet
					printf("Creating new SenderList and adding sender to it.\n");
					//	cursor->mbSender = (struct sender *) malloc(sizeof(struct sender));
					senderToAdd = (struct sender *) malloc(sizeof(struct sender));
					//initialize both elements of sender struct
					senderToAdd->id=m_in.m1_i1;
					senderToAdd->next=NULL;
					cursor->mbSender = senderToAdd;
					return 1;
				}
				else{//senders exist, add to end of list
					printf("Adding sender to end of list.\n");
					cursor->mbSender = mbSenderFront;
					if(cursor->mbSender->id==m_in.m1_i1){//if only one element in list
						printf("Sender already exists.\n");
						return -1;//senderToAdd already exists
					}
					while(cursor->mbSender->next!=NULL){//get to the end of the sender list //cursor->mbSender->next!=NULL
						cursor->mbSender=cursor->mbSender->next;
						if(cursor->mbSender->id==m_in.m1_i1){
							printf("Sender already exists.\n");
							return -1;//senderToAdd already exists
						}
					}
					senderToAdd = (struct sender *) malloc(sizeof(struct sender));
					senderToAdd->id=m_in.m1_i1;
					senderToAdd->next=NULL;//new end of sender list
					cursor->mbSender->next = senderToAdd;
					/*printf("printing sender list after addition  ");
					cursor->mbSender = mbSenderFront;
					while(cursor->mbSender !=NULL){
						printf("sender = %d  \n",cursor->mbSender->id);
						cursor->mbSender  = cursor->mbSender->next;
					}*/
					cursor->mbSender = mbSenderFront;
					return 1;
				}

			}//end of if to check mailbox id
			else{
				cursor=cursor->next;
			}
		}//end of while
	}//end of if
	return -1;
}

/*************************Remove Sender from Mailbox*****************************/
int do_removeSender()
{
//	printf("Removing Sender do_removeSender\n");
	//remove sender m_in.m1_i1 from the list of senders of mailbox who_e
	struct mailBox *cursor=mailboxFront;
	struct sender *senderToRemove, *temp,  *mbSenderFront;
	int ownerID = m_in.m1_i3;
	if(cursor!=NULL)
	{//if mailbox exists
		while(cursor!=NULL)
		{
			if(cursor->owner == ownerID){//if correct mailbox found
				//loop needs to be executed at least once
				mbSenderFront = cursor->mbSender;
				temp = mbSenderFront;
				if(cursor->mbSender == NULL){
					printf("Cannot remove sender. No senders exist.\n");
					return -1;
				}
				cursor->mbSender = mbSenderFront;
				if(cursor->mbSender->id==m_in.m1_i1)//deal with first element of list separately
				{//WORKS!!!
					printf("Found sender %d to remove.\n", cursor->mbSender->id);
					/*printf("printing sender list before removal\n");
					while(temp!=NULL){
						printf("sender = %d  ",temp->id);
						temp = temp->next;
					}*/
					senderToRemove = cursor->mbSender;
					cursor->mbSender = cursor->mbSender->next;
					free(senderToRemove);
					senderToRemove = NULL;
					return 1;
				}
				else{
			//		printf("running loop till we find the correct sender\n");
					/*printf("printing sender list before removal\n");
					temp = 	mbSenderFront;
					while(temp!=NULL){
						printf("sender = %d  ",temp->id);
						temp = temp->next;
					}*/

					int found = 0;
					struct sender * prev;
					temp = cursor->mbSender;
					while(temp->next!=NULL){
						if(temp->next->id ==m_in.m1_i1){
							printf("Found sender %d to delete.\n", temp->next->id);
							prev = temp;
							found = 1;
							break;
						}
						temp=temp->next;
					}
					if(found==1){
						prev = temp->next;
						temp->next = prev->next;
						free(prev);
						prev = NULL;

						/*printf("printing sender list after removal\n");
						temp = mbSenderFront;
						while(temp!=NULL){
							printf("sender = %d  ",temp->id);
							temp = temp->next;
						}*/
						return 1;
					}
					if(found==0){
						printf("Sender not found.\n");
						return -1;
					}
				}
			}//end of if for checking which mailbox it is
			else{//not correct mailbox, go to next one
				cursor=cursor->next;
			}
		}//end of while
	}
	return -1;//if control comes here, its an error
}

/*************************Check if Sender is on Allowed Senders List*****************************/
int checkPermissions(struct mailBox *mailperm, int ownerID)
{
//	printf("Checking Permissions.\n");
	if(mailperm->mbSender==NULL){
		printf("No senders allowed to post.\n");
		return -1; //return -1 for error
	}
	else{
		printf("Checking Permissions. ");
		while(mailperm->mbSender!=NULL){
			if(mailperm->mbSender->id==ownerID){
				printf("OK.\n");
				return 1;
			}
			else{
				mailperm->mbSender=mailperm->mbSender->next;
			}
		}//end of while
		printf("Not OK.\n");
		return -1; //return -1 for error
	}//end of else
}

/*************************Insert to Mailbox of Receiver*****************************/
int do_msend()
{
	int ownerID = m_in.m1_i3;
//	printf("Sending Message do_msend\n");
	if(mailboxFront == NULL){
		printf("Mailbox not set up.\n");
		return -1;
	}
	else{
	//	printf("Mailbox list found.\n");
		int m1i2size = m_in.m1_i2*sizeof(int);
		int *tempReceiver=(int *)malloc(m1i2size);
		struct mailBox *cursor;
		struct messageList *tempML;
		int mailboxOfRecExists = 0;
		
        //m1_p2 contains the list of receivers, m1_i2 contains the number of receivers
		
        sys_datacopy(who_e, (vir_bytes)m_in.m1_p2, SELF, (vir_bytes)tempReceiver, m1i2size);
		
        for(int i=0;i<m_in.m1_i2;i++){
			
  //          printf("out tempReceiver[i] %d \n", tempReceiver[i]);
			cursor=mailboxFront; //cursor
            
			while(cursor!=NULL){//check that mailbox exists
	//			printf("cursor owner %d \n", cursor->owner);

				if(cursor->owner==tempReceiver[i]){
					mailboxOfRecExists = 1;
		//			printf("mailbox Of Receiver %d", mailboxOfRecExists);
				}
				else{
					cursor = cursor->next;//try next mailbox
					continue;
				}

				if(mailboxOfRecExists==1){
					printf("Mailbox of Receiver Exists.\n");
					if(cursor->numMes>=BUFFERSIZE){//buffer capacity = BUFFERSIZE
						printf("Mailbox buffer full.\n");
						return -1;
						//discard the message as blocking sender can cause deadlocks
					}

					//Check if the process can (has the right to) post a message into this receiver mailbox
					if(checkPermissions(cursor, ownerID)==-1){
						printf("Sender does not have permission to send to this Receiver.\n");
						return -1;
					}
					//printf("cursor owner = %d, tempReceiver[i] = %d\n", cursor->owner, tempReceiver[i]);

					if((cursor->numMes<BUFFERSIZE))//would come to this line only if (checkPermissions(cursor)==1)
					{
			//			printf("Sender has permission to send to this Receiver\n");
						tempML = cursor->mbList;
						if(tempML==NULL){ //create new messagelist
							printf("Message list did not exist. Creating it now.\n");
							tempML = (struct messageList *)malloc(sizeof(struct messageList));
							tempML->next=NULL;
						}
						else{//add to existing mailbox
							printf("Message list exists. Adding to it now\n");
							while(tempML->next != NULL){//get to end of list
								tempML=tempML->next;
							}
							tempML->next=(struct messageList *)malloc(sizeof(struct messageList));
							tempML->next->next=NULL;
							tempML=tempML->next;
						}


						/*	circular queue implmentation
						    tempML = cursor->mbList;
							tempML = malloc(sizeof(struct messageList));
							tempML->next=NULL;
							if(rear==NULL) //create new mailbox
								front=rear=tempML;
							else{//add to existing mailbox
								rear->next=tempML;
								rear = tempML;
							}
							rear->next=front;//complete the loop*/

						memcpy(&tempML->m,&m_in,sizeof(message));
						tempML->m.m1_p1 = (char *)malloc(MESSAGESIZE);//actual message

						if(m_in.m1_i1>MESSAGESIZE){
							printf("Message is too long, truncating to %d characters.\n", MESSAGESIZE);
							m_in.m1_i1 = MESSAGESIZE;
						}
						if((sys_datacopy(who_e, (vir_bytes)m_in.m1_p1, SELF, (vir_bytes)tempML->m.m1_p1, m_in.m1_i1+1))== 0)
						{
							printf("Copy OK.\n");
						}

		//				printf("Message %s was copied from\n",m_in.m1_p1); //Addresses
		//				printf("Message %s was copied into\n",tempML->m.m1_p1);

						tempML->sender=ownerID;
			//			printf("message contains: %s\n",tempML->m.m1_p1); // Message contains
                        
                        struct messageList *mbListFront;
                        mbListFront = cursor->mbList;
						if(cursor->mbList==NULL){
							printf("Creating message list and adding message to it now.\n");
							cursor->mbList=tempML;
                            cursor->numMes++;
                            mbListFront = cursor->mbList;
                            return 1;
						}
						else{ //already some messages in the list
							printf("Message list exists. Adding to it now.\n");
                            while(cursor->mbList->next != NULL){
                                cursor->mbList = cursor->mbList->next;
                            }
                            cursor->mbList->next = tempML;
							cursor->mbList->next->next = NULL;
                            cursor->numMes++;
                            cursor->mbList = mbListFront;
                            return 1;
						}

						// This part handles the deadlocks, if the receiver process was suspended, it is resumed
						struct procDeadlock * temp=deadlockOne;
						while(temp != NULL && temp->deadlocked != tempReceiver[i]){
							temp = temp->next;
						}
						if(temp!=NULL)
							sys_resume(temp->deadlocked);
						break;

					}// end ofif((cursor->numMes<BUFFERSIZE))
					else{
						cursor=cursor->next;
					}
				}//end of if(mailboxOfRecExists==1)
			}//end of while
            return -1;
		}//end of for loop
	}
}

/*************************Remove from Mailbox of Receiver*****************************/
int do_mreceive()
{
//	printf("Receiving Message do_mreceive\n");
	struct mailBox *cursor=mailboxFront;
	char *sourceAddress, *returnAddress;
	struct messageList *tempML;
    struct messageList *delML, *tempMLFront;
	int found=0;
	int ownerID = m_in.m1_i3;
	returnAddress=m_in.m1_p1;

	if(mailboxFront == NULL){
		printf("Mailbox not set up.\n");
		return -1;
	}
	else{
//		printf("Mailbox list found. Looking for mailbox id %d \n",ownerID);
    
		// Deal with deadlocked process
		if(deadlockOne != NULL)
		{
			struct procDeadlock * temp;
			temp = deadlockOne;

			if(temp->deadlocked==ownerID)
			{
				returnAddress=temp->procAddress;
				m_in.m1_i1=temp->deadlockedSource;
				deadlockOne=temp->next;
                temp = NULL;
				free(temp);
			}
			else
			{
				while(temp->next!=NULL)
				{
					if(temp->next->deadlocked==ownerID)
					{
						returnAddress=temp->next->procAddress;
						m_in.m1_i1=temp->next->deadlockedSource;
						temp->next=temp->next->next;
                        temp->next = NULL;
						free(temp->next);
						break;
					}
					temp=temp->next;
				}
			}
		}


		if(cursor!=NULL){//same as mailboxFront!=NULL
			while(cursor!=NULL){
				tempML = cursor->mbList;//message list
                tempMLFront = tempML;
				if(cursor->owner==ownerID){   //mailbox found
					printf("Found %d mailbox.\n",ownerID);
					if(tempML!=NULL){
						sourceAddress=tempML->m.m1_p1;
						if(tempML->sender==m_in.m1_i1){
							printf("Found %d sender.\n",m_in.m1_i1);
							found=1;
							sourceAddress=tempML->m.m1_p1;//get id of sender
						}
						else{
						//	printf("Looking for %d sender.\n",m_in.m1_i1);
							while(tempML->next!=NULL){
								if(tempML->next->sender==m_in.m1_i1){
									printf("Found %d sender.\n",m_in.m1_i1);
									found=1;
									sourceAddress=tempML->next->m.m1_p1;
									break;
								}
								else
								{
									tempML=tempML->next;
								}
							}
						}
					}//end of tempML if


					if((tempML == NULL)||(found==0)||(m_in.m1_i1 == 0))
					{
						struct procDeadlock *suspProc = malloc(sizeof(struct procDeadlock));
						//blockProc = NULL;

						suspProc->deadlocked=ownerID; //Give the id of the suspended process
						suspProc->procAddress = m_in.m1_p1; // Address of the suspended process
						suspProc->deadlockedSource = m_in.m1_i1;
						suspProc->next=deadlockOne; // Place this suspended process in the first place of the queue
						deadlockOne = suspProc; // Upload tthe first deadlocked process in the queue
						sys_stop(who_e);

						printf("No message in the mailbox, process suspended.\n");
						return -1;
						//break;
					}

					// Action if message is received
					if(sys_datacopy(SELF, (vir_bytes)sourceAddress, who_e, (vir_bytes)returnAddress, strlen(sourceAddress)+1) == 0){
						printf("Message Received.\n");
					}
					else
					{
						printf("Error in the transfer from mailbox to process.\n");
						return -1;
					}

                    if (found==1){// Delete messages.
                        printf("Deleting message that was received from pending messages list.\n");

                        /*tempML = cursor->mbList;
                        while(tempML!=NULL){
                        	printf("message contains %s", tempML->m.m1_p1);
                        	tempML = tempML->next;
                        }*/

                        tempML = cursor->mbList;
                        if(m_in.m1_i1 == tempML->sender)
                        {
                            printf("Deleting message.\n");
                            delML=tempML;
                            tempML=tempML->next;
                     //       tempMLFront = tempML;
                            cursor->mbList = tempML;
                            free(delML);
                            delML = NULL;
                            return 1;
                        }
                        else{
                  //          printf("Looking for message to delete\n");
                            struct messageList *prev;
                            int foundML = 0;
                            while(tempML->next!=NULL){
                                if(tempML->sender==m_in.m1_i1){
                                    printf("Found message to delete. ");
                                    prev=tempML;
                                    foundML =1;
                                    break;
                                }
                                else{
                                    tempML = tempML->next;
                                }
                            }
                            if(foundML == 1){
                                prev = tempML->next;
                                tempML->next = prev->next;
                                cursor->mbList = tempML;
                                printf("Message Deleted.\n");
                                free(prev);
                                prev=NULL;
                                return 1;
                            }
                            else
                            {
                                printf("Message not found.\n");
                                return -1;
                            }
                        }
					}
				}//end of cursor owner if
			}//end of while
		}//end of cursor!=NULL if
		else{
			printf("Mailbox does not exist.\n");
			return -1;
		}
	}//end of else
}
