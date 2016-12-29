#include <lib.h>
#include <unistd.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/types.h>//for truncate
#include <minix/type.h>
#include <minix/ipc.h>//for message struct
#define BUFFERSIZE 5
#define MESSAGESIZE 1024
#define GROUPSIZE 64
#define NUMGROUPRECEIVERS 64

typedef struct sender //Linked list of senders
{
	int id;
	struct sender *next;
}sender;

typedef struct receiver //Linked list of subscribers
{
	int id;
	struct receiver *next;
}receiver;

//message structure used from ipc.h

typedef struct messageList //Circular queue of messages
{
	message m;//using type mess_1
	int sender;
	struct messageList *next;
}messageList;

typedef struct mailBox //Linked list of mailboxes
{
	int numMes;
	int owner;
	struct sender *mbSender;
	struct receiver *mbReceiver;
	struct messageList * mbList;
	struct mailBox * next;
}mailBox;


//Deadlock handling
typedef struct procDeadlock
{
    int deadlocked;
    int deadlockedSource;
    char *procAddress;
    struct procDeadlock *next;
}procDeadlock;

