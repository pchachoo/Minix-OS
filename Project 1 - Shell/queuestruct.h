struct queuenode{
	char *command; //points to array of characters composing a shell command
	int priority;
	struct queuenode *ptr;
};

struct queuenode * firstelement();
int qsize();
void qenqueue(char *command, int priority);
char * qdequeue();
void qcreate();
void qdelete(struct queuenode *qcmdlist);
char * firstcommand(int prioritychk);
void qdisplay();
struct queuenode * tokenizer (char *argv);
