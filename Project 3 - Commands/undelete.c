//
//  test.c
//  
//
//  Created by Thibault Timmerman on 23/11/2016.
//
//

#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <fts.h>
#include <grp.h>
#include <locale.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <lib.h>
#include <signal.h>
#include "HashTable.h"


void initMessage(message *mes)
{
    mes->m1_i1=0;
    mes->m1_i2=0;
    mes->m1_i3=0;
    mes->m1_p1=NULL;
    mes->m1_p2=NULL;
    mes->m1_p3=NULL;
    
}


int recover_file(int num, char * filename, int dir_inode, int dev){
    message m;
    initMessage(&m);
    m.m1_i1 = num;
    m.m1_i2 = dir_inode;
    m.m1_i3 = dev;
    m.m1_p1 = filename;
    
    //char fullpath[100];
    
    //copy_name(100, fullpath);
    
    //printf("%lu /n", vname);
    
    //job.m_in.name = filename;
    
    //printf("path sent = %s\n", m.m1_p1);
    //printf("before system call vfs\n");
    int x = _syscall(VFS_PROC_NR, 108, &m);
    //printf("path sent = %s\n", m.m1_p1);
    //printf("after system call vfs, x = %d\n", x);
    
    return x;
}


int main(int argc, char *argv[]){
    int inode;
    char * filename;
    char curDir[1024];
    //char dirPath[1025];
    char fullPath[1100];
    struct stat buf;
    
    char option = 0;
    
    
    if(argc < 2){
        
        printf("Do you want to recover all files of the directory? Press 1\n");
        printf("Do you want to recover a file using its inode? Press 2\n");
        printf("Beta test for the file test.c Press 3.\n");
        scanf("%s", &option);
        
        switch(option){
            case '1':
            {
                int * array;
                int i =0;
                printf("Recover all files of the directory\n");
                if (getcwd(curDir, sizeof(curDir)) != NULL){
                    stat(curDir, &buf);
                    array = searchOnPath(curDir);
                    while(array[i] != -1){
                        filename = returnFileName(array[i]);
                        int x = recover_file(array[i], filename, buf.st_ino, buf.st_dev);
                    }
                    
                }
                else {
                    printf("Cannot get the current directory path");
                    return 0;
                }
                break;
            }
            case '2':
            {
                printf("Enter inode of the file to recover\n");
                scanf("%d", &inode);
                
                if (getcwd(curDir, sizeof(curDir)) != NULL){
                    
                    
                    stat(curDir, &buf);
                    
                    filename = returnFileName(inode);
                    //strcpy(fullPath, strcat(strcat(curDir, "/"), argv[1]));
                    
                    //printf("full path = %s \n", fullPath);
                    
                    //printf("calling recover file\n");
                    
                    //strcpy(filename, argv[1]);
                    
                    int x = recover_file(inode, filename, buf.st_ino, buf.st_dev);
                    
                    return 1;
                }
                else {
                    printf("Cannot get the current directory path");
                    return 0;
                }
                break;
            }
            case '3':
            {
                printf("Enter the inode of the test.c file\n");
                scanf("%d", &inode);
                
                if (getcwd(curDir, sizeof(curDir)) != NULL){
                    stat(curDir, &buf);
                    int x = recover_file(inode, "test.c", buf.st_ino, buf.st_dev);
                }
                else {
                    printf("Cannot get the current directory path");
                    return 0;
                }
                break;
            }
        }
    }
    else if (argc == 2){
        
        if (getcwd(curDir, sizeof(curDir)) != NULL){
            stat(curDir, &buf);
            char * fullPath = combineStrings(curDir, argv[1]);
            inode = searchOnData(fullPath);
            
            int x = recover_file(inode, argv[1], buf.st_ino, buf.st_dev);
        }
        else {
            printf("Cannot get the current directory path");
            return 0;
        }

        
        
    }
    else {
        printf("Too many arguments");
        return 0;
    }
    //return 1;
}
