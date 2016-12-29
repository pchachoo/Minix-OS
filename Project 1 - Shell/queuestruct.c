#include "queuestruct.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>

int count;
struct queuenode *first, *qcmd; //*last

struct queuenode * firstelement(){
	return first;
}

char * firstcommand(int prioritychk){
	if(first->priority==prioritychk) //if first element in queue has given priority
		return first->command;
	else
		return NULL;
}

int qsize(){
	return count;
}

void qenqueue(char *command, int priority){

	struct queuenode *newnode = (struct queuenode*)malloc(sizeof(struct queuenode));

	int incommandlen = strlen(command);
	newnode->command = (char*)malloc(incommandlen);
	strcpy(newnode->command , command); //copy command
	newnode->priority = priority; //set priority
	newnode->ptr = NULL; //set next pointer to NULL
	//case when it should be inserted first
	if((first==NULL) || newnode->priority < first->priority){//empty queue or higher priority; lower number = higher priority
		newnode->ptr = first;
		first = newnode;
	}
	else {
		qcmd = first;
		while((qcmd->ptr!=NULL) && newnode->priority >= qcmd->ptr->priority){
			//using >= so that case with equal priority goes to end of the list
			qcmd = qcmd->ptr; //go to next element in the queue
		}
		//after finding the element with highest priority less than new node's, insert node just before it
		newnode->ptr = qcmd->ptr;
		qcmd->ptr = newnode;

	}//end of else
	count++;
}

char * qdequeue(){
	struct queuenode *dqnode;
	char * dqcmd = NULL; //TODO
	if(first==NULL)
		printf("Error: Cannot dequeue from empty queue.\n");
	else{
		dqnode = first;
		dqcmd = (char *)malloc(strlen(dqnode->command));
		strcpy(dqcmd, dqnode->command);
		first = first->ptr; //remove first element of queue - always FIFO
		free(dqnode); //free the memory
		dqnode = NULL;
		count--;
	}
	return dqcmd;
}

void qdelete(struct queuenode *qcmdlist){
	struct queuenode *qdelnode;// qcmdlist = first
	while(qcmdlist!=NULL){
	qdelnode = qcmdlist;
	qcmdlist = qcmdlist->ptr;
	free(qdelnode);
	qdelnode = NULL;
	count--;
	}
	first = NULL;

}

void qcreate(){
	first = NULL;
	count = 0;
}

void qdisplay(){
	printf("This is the queue:\n");
	struct queuenode *qfordisplay;
	if(first == NULL)
	{
		printf("The queue is empty");
		return;
	}
	qfordisplay = first;
	while(qfordisplay!=NULL){
		printf("%s ", qfordisplay->command);
		printf("%d \n", qfordisplay->priority);
		qfordisplay = qfordisplay->ptr;
	}
}
