//
// Created by ivan on 04.06.15.
//
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include "processing.h"

#define ERROR(code, name) if ((code) < 0) {perror(name); _exit(-1);}

#define MSGLEN 16384
#define DIRLEN 256

int csd;

int processing_apply()
{
    int i;
    fd_set set;
    struct sockaddr_in clientaddr;
    struct timeval timeout;

    timeout.tv_sec=1;
    timeout.tv_usec=0;

    int clientlen=sizeof(clientaddr);

    FD_SET(sd, &set);
    ERROR(select(sd+1, &set, NULL, NULL, &timeout), "select");
    ERROR(csd=accept(sd, (struct sockaddr*)&clientaddr, &clientlen), "accept");
    ERROR(pthread_mutex_lock(&p_wm->mutex), "mut_lck");

    for (i=0; i < p_wm->processing_n; i++)
        if (p_wl[i].pid == getpid())
        {
            p_wl[i].status=1;
            break;
        }

    ERROR(pthread_mutex_unlock(&p_wm->mutex), "mut_unl");

    return 0;
}

int processing_init(int cpid)
{
    int i;

    for (i=0; i < p_wm->processing_n; i++)
        if (p_wl[i].status == -1)
        {
            p_wl[i].pid=cpid;
            p_wl[i].status=0;
        }

    return 0;
}

int processing_fire()
{
    int i;

    ERROR(pthread_mutex_lock(&p_wm->mutex), "mut_lck");

    for (i=0; i < p_wm->processing_n; i++)
        if (p_wl[i].pid == getpid())
        {
            p_wl[i].status=0;
            break;
        }

    ERROR(pthread_mutex_unlock(&p_wm->mutex), "mut_ulk");

    return 0;
}

int find_offset(const char* dirname)
{
    int i, offset=0;

    for (i=strlen(dirname)-1; i >=0; i--)
        if (dirname[i] == 13 || dirname[i] == 10)
            offset++;
        else
            return offset;

    return offset;
}

void print_dirents(char* dirname)
{
    DIR *dir;
    struct dirent *entry;

    char* nl="\n";
    char* errop="Unable to open\n";
    char reply[MSGLEN];
    int reply_l=0;
    char eof[1];
    *eof=EOF;

    dirname[strlen(dirname)-find_offset(dirname)]='\0';

    dir=opendir(dirname);
    if (!dir)
    {
        strcpy(reply, dirname);
        strcat(reply, nl);
        strcat(reply, errop);
        strcat(reply, nl);
        reply_l=strlen(dirname)+strlen(errop)+2*strlen(nl);
        strcat(reply, eof);
        reply_l+=1;

        ERROR(send(csd, reply, reply_l, 0), "send");

        return;
    }

    strcpy(reply, dirname);
    strcat(reply, nl);
    reply_l=strlen(dirname)+strlen(nl);

    while ( (entry=readdir(dir)) != NULL )
    {
        if (reply[0] == '\0')
            strcpy(reply, entry->d_name);
        else
            strcat(reply, entry->d_name);

	strcat(reply, nl);
	reply_l+=strlen(entry->d_name)+strlen(nl);
        if (MSGLEN-reply_l < DIRLEN)
        {

            ERROR(send(csd, reply, reply_l, 0), "send");
            ERROR(memset(reply, 0, reply_l), "memset");
            reply_l=0;
        }
    }

    strcat(reply, nl);
    reply_l+=strlen(nl);
    strcat(reply, eof);
    reply_l+=1;

    ERROR(send(csd, reply, reply_l, 0), "send");
    ERROR(closedir(dir), "close");
}

int processing_service()
{
    int readb;
    char msg[MSGLEN];

    while ( (readb=recv(csd, msg, MSGLEN, 0)) > 0)
    {
        msg[readb]='\0';
        print_dirents(msg);
    }

    if (readb == -1)
    {
        perror("recv");
        _exit(-1);
    }

    return 0;
}

