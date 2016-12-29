/*
 * HashTable.h
 *
 *  Created on: Nov 20, 2016
 *      Author: pchachoo
 */

#ifndef HASHTABLE_H_
#define HASHTABLE_H_



#endif /* HASHTABLE_H_ */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define SIZE 3 //TODO change for different number of deleted items
//#define MAXDEL 5 //keep last 100 deleted items in table
extern int hashSize;

struct DataItem {
   char *data;
   int key;
};

struct DataItem *searchHashTable(int key);
bool searchOnInode(int key);
int searchOnData(char * path);
int comparePath(char * path, char * data);
int * searchOnPath(char * path);
char * returnFileName(int key);
void insertToHashTable(int key,char * data);
struct DataItem* deleteFromHashTable(int keyDel);
void displayHashTable();
struct DataItem* deleteFromHash();
void printInodeArray();
void insertToHash(int key,char * data) ;
char* combineStrings(char* path, char* filename);


/*struct inodeTable {
   char * inodeNum;
   int key;
};

struct pathTable {
   char *filePath;
   int key;
};

struct DataItem {
   int data;
   int key;
};*/
