//
//  recover.c
//  
//
//  Created by Thibault Timmerman on 23/11/2016.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <lib.h>
#include <sys/types.h>

#include "fs.h"
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/statvfs.h>
#include <minix/vfsif.h>
#include <minix/com.h>
#include <minix/ipc.h>
#include <minix/const.h>
#include <minix/endpoint.h>
#include <minix/u64.h>
#include <minix/vfsif.h>
#include "fproc.h"
#include "vmnt.h"
#include "vnode.h"
#include "path.h"
#include "param.h"


int do_recover(){
    
    //char fullpath[PATH_MAX]; //= m_in.m1_p1;
    //strcpy(fullpath, m_in.m1_p1);
    struct vmnt *vp;
    cp_grant_id_t grant_id;
    size_t len;
    char *filename;
    
    filename = "test.c";
    
    //filename = m_in.m1_p1;
    //printf("m1_p1 = %s", m_in.m1_p1);
    //printf("filename 2 %s", filename2);
    
    //strcpy(filename, m_in.m1_p1);
    
    //int i_num = m_in.m1_i1;
    
    printf("Do recover system call\n");
    //printf("do recover, inode received = %d\n", i_num);
    
    for (vp = &vmnt[0]; vp < &vmnt[NR_MNTS]; ++vp) {
        if(vp->m_dev == m_in.m1_i3){
            //printf("vp mdev %d \n", vp->m_dev);
            
            //printf("filename received %s \n", filename);
            
            len = strlen(filename)+1;
            grant_id = cpf_grant_direct(vp->m_fs_e, (vir_bytes) filename, len, CPF_READ);
            if (grant_id == -1){
                panic("req_create: cpf_grant_direct failed");
            }
            
            // Making the message for MFS
            message m;
            m.m_type = REQ_RECOVER;
            m.REQ_INODE_NR = m_in.m1_i1; // Inode of file to recover
            m.REQ_DEV = m_in.m1_i3; // Device
            m.REQ_DIR_INO = m_in.m1_i2; // Inode of parent directory
            m.REQ_GRANT = grant_id; // Information concerning filename
            m.REQ_PATH_LEN = len;
            
            // Call fs_recover
            printf("do recover before send/rec\n");
            int r = fs_sendrec(vp->m_fs_e, &m);
            printf("after send/rec\n, r = %d", r);
        }
    }
    
    return 0;
    
}

