/*
 * HashTable.c
 *
 *  Created on: Nov 14, 2016
 *      Author: pchachoo
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include "HashTable.h"

struct DataItem* hashArray[SIZE];
struct DataItem* dummyItem;
int hashSize = 0;
int inodeArray[SIZE];
int arrInodes[SIZE];

int hashCode(int key) {
	return key % SIZE;
}

struct DataItem *searchHashTable(int key) {
	//get the hash
	int hashIndex = hashCode(key);
	int tempsize = 0;
	//move in array until an empty
	while(hashArray[hashIndex] != NULL && tempsize<SIZE) {

		if(hashArray[hashIndex]->key == key)
			return hashArray[hashIndex];
		//go to next cell
		++hashIndex;
		//wrap around the table
		hashIndex %= SIZE;
		tempsize++;
	}

	return NULL;
}

bool searchOnInode(int key) {
	//get the hash
	int hashIndex = hashCode(key);
	int tempsize = 0;
	//move in array until an empty
	while(hashArray[hashIndex] != NULL && tempsize<SIZE) {

		if(hashArray[hashIndex]->key == key)
			return true;
		//go to next cell
		++hashIndex;
		//wrap around the table
		hashIndex %= SIZE;
		tempsize++;
	}
	return false;
}

int searchOnData(char * data){
	int i = 0;
	for(i = 0; i<hashSize; i++) {
		if(strcasecmp(hashArray[i]->data,data)==0){
			printf("HashTable entry found: (%d,%s)\n",hashArray[i]->key,hashArray[i]->data);
			return hashArray[i]->key;
		}
	}
	return -1;
}

int comparePath(char * path, char * data){
//	printf("string length of path = %d", strlen(path));
	return(strncmp(data,path,strlen(path))); //0 if strings match, 1 if they dont
}

int * searchOnPath(char * path){
	int i = 0;
	int aiCounter = 0;
	for(i=0;i<SIZE;i++){
		arrInodes[i] = -1;
	}
	for(i = 0; i<hashSize; i++) {
		if(comparePath(path,hashArray[i]->data)==0){
			printf("HashTable entry found: (%d,%s)\n",hashArray[i]->key,hashArray[i]->data);
			arrInodes[aiCounter] = hashArray[i]->key;
			aiCounter++;
		}
	}
	return arrInodes;
}

char * returnFileName(int key){
//	char * filename;
	char ch = '/';
	int hashIndex = hashCode(key);
	int tempsize = 0;
	//move in array until an empty
	while(hashArray[hashIndex] != NULL && tempsize<SIZE) {
		if(hashArray[hashIndex]->key == key){
			char * pathLoc = strrchr(hashArray[hashIndex]->data, ch);
			printf("/ location = %s",pathLoc);
			if (pathLoc[0] == '/')
			    memmove(pathLoc, pathLoc+1, strlen(pathLoc));
			printf("/ location = %s",pathLoc);
		//	strncpy(filename,pathLoc+1, strlen(pathLoc)-1);
	//		strcpy(filename,pathLoc);
			return pathLoc;
		}
		//go to next cell
		++hashIndex;
		//wrap around the table
		hashIndex %= SIZE;
		tempsize++;
	}
    return NULL;
}

void insertToHashTable(int key,char * data) {

	struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
	item->data = (char *)malloc(strlen(data));
	strcpy(item->data,data);
	item->key = key;
	int tempsize = 0;
	//   if(hashSize>=MAXDEL){
	//	   printf("Too many files deleted. Replacing LRU file from undelete table");//TODO
	//	   delete();
	//   }
	//get the hash
	int hashIndex = hashCode(key);

	//move in array until an empty or deleted cell
	while(hashArray[hashIndex] != NULL && hashArray[hashIndex]->key != -1 && tempsize<SIZE) {
		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= SIZE;
		tempsize++;
	}

	hashArray[hashIndex] = item;
	hashSize++;
}

struct DataItem* deleteFromHashTable(int keyDel) {
	dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
	dummyItem->data = "";
	dummyItem->key = -1;
	//get the hash
	int hashIndex = hashCode(keyDel);
	if(hashSize<1){
		printf("no elements to delete");
		return NULL;
	}
	//move in array until an empty
	while(hashArray[hashIndex] != NULL) {

		if(hashArray[hashIndex]->key == keyDel) {
			struct DataItem* temp = hashArray[hashIndex];

			//assign a dummy item at deleted position
			hashArray[hashIndex] = dummyItem;
	//		printf("File deleted.\n");
			hashSize--;
			return temp;
		}

		//go to next cell
		++hashIndex;

		//wrap around the table
		hashIndex %= SIZE;
	}

	return NULL;
}

void displayHashTable() {
	int i = 0;
	printf("Hashtable is as follows:\n");
	for(i = 0; i<SIZE; i++) {

		if(hashArray[i] != NULL && hashArray[i]->key!=-1)
			printf("(%d,%s)\n",hashArray[i]->key,hashArray[i]->data);
		else
			printf(" ~~ ");
	}

	printf("\n");
}

//call delete when a file is recovered and hence needs to be removed from hash table
struct DataItem* deleteFromHash() {//TODO call when a file is undeleted
	struct DataItem* temp = NULL;
	if(hashSize>=1){//hash contains at least one element
		int key = inodeArray[0];
		//move all elements of the array forward by 1 space as first element is being deleted
		for(int i=0;i<hashSize-1;i++){//copy elements 1 to 99 into array indices 0 to 98
			inodeArray[i] = inodeArray[i+1];
		}
		temp = deleteFromHashTable(key);//decrements hashSize on successful deletion
		printf("File %d deleted: %s\n", temp->key, temp->data);
	}
	else
		printf("hashtable is empty");
	return temp;
}

void printInodeArray(){
	for(int i=0;i<hashSize;i++)
		printf("inodeArray[%d] = %d",i,inodeArray[i]);
}

void insertToHash(int key,char * data) {
	//increment hashSize on successful insertion
	if(hashSize<SIZE){//there is space in the hash table
		inodeArray[hashSize] = key;
		insertToHashTable(key, data);//will increment hashSize
	}
	else{//we need to make space in the table by deleting LRU table entry as hashSize=100
		//delete element with oldest inode
		deleteFromHash();//always deletes the first element in the inodeArray
		//space has been created in hash table, so add new element
		inodeArray[hashSize] = key;//new element should be inserted at index 99
		insertToHashTable(key, data);
	}
}

char* combineStrings(char* path, char* filename){
	char * combinedString = NULL;
	char * slash = "/";
	strcpy(combinedString,path);
	strcat(combinedString,slash);
	strcat(combinedString,filename);
	return combinedString;
}
