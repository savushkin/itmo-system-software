//
// Created by ivan on 04.06.15.
//
#include <sys/loadavg.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <malloc.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include "processing.h"

#define ERROR(code, name) if ((code)<0) {perror(name); _exit(-1);}
#define PORT 18219
#define N 5 // minimum of free processings
#define K 15 // maximum of free processings
#define A 5 // default attempt number
#define KEYWL 1821901
#define KEYM 1821902

int sd;
int shmidwl, shmidm;
processing_info* p_wl;
processing_mut* p_wm;
pthread_mutex_t mutex;

int server_init()
{
    struct sockaddr_in servaddr;

    ERROR(sd=socket(AF_INET, SOCK_STREAM, 0), "socket");
    ERROR(memset(&servaddr, 0, sizeof(servaddr)), "memset");

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=INADDR_ANY;
    servaddr.sin_port=htons( PORT );

    ERROR(bind(sd, (struct sockaddr*)&servaddr, sizeof(servaddr)), "bind");
    ERROR(listen(sd, BACKLOG), "listen");

    ERROR(shmidm=shmget(KEYM, sizeof(processing_mut), IPC_CREAT|0660), "shmget");
    ERROR(p_wm=(processing_mut*)shmat(shmidm, NULL, 0), "shmat");
    ERROR(memset(p_wm, 0, sizeof(processing_mut)), "memset");
    ERROR(pthread_mutex_init(&p_wm->mutex, NULL), "mut_init");

    ERROR(shmidwl=shmget(KEYWL, sizeof(processing_info)*BACKLOG, IPC_CREAT|0660), "shmget");
    ERROR(p_wl=(processing_info*)shmat(shmidwl, NULL, 0), "shmat");
    ERROR(memset(p_wl, -1, sizeof(processing_info)*BACKLOG), "memset");

    return 0;
}

int enlarge_pool()
{
    int i, cpid;

    for (i=0; i < M; i++)
    {
        cpid=fork();

        if (cpid < 0)
        {
            perror("fork");
            return(-1);
        }
        else if (cpid == 0) // processingS!!!
        {
            for (;;)
            {
                processing_apply();
                processing_service();
                processing_fire();
            }
        }
        else
        {
            p_wm->processing_n++;
            processing_init(cpid);
        }
    }

    return 0;
}

int shrink_pool(int kill_n)
{
    int i, j=p_wm->processing_n-1, k=0;

    for (i=0; i < kill_n; i++)
        for(; j >= 0; j--)
            if (p_wl[j].status == 0)
            {
		printf("%d %d\n", i+1, p_wl[j].pid);
                ERROR(kill(p_wl[j].pid, SIGKILL), "kill");
                p_wl[j].status=-1;
                break;
            }
            else if (p_wl[j].status == 1)
            {
                for (; k < p_wm->processing_n-1; k++)
                    if (p_wl[k].status == 0)
                    {
                        printf("%d %d\n", i+1, p_wl[k].pid);
                        ERROR(kill(p_wl[k].pid, SIGKILL), "kill");
                        p_wl[k].pid=p_wl[j].pid;
                        p_wl[k].status=1;
                        p_wl[j].status=-1;
                        break;
                    }
                break;
            }

    p_wm->processing_n-=kill_n;

    for (i=0; i < kill_n; i++)
        ERROR(wait(NULL), "wait");

    return 0;
}

int check_pool()
{
    int i, free_n=0;

    ERROR(pthread_mutex_lock(&p_wm->mutex), "вася курнул, вася торчок");

    for (i=0; i < p_wm->processing_n; i++)
    {
        //printf("%d pid: %d, status: %d\n", i+1, p_wl[i].pid, p_wl[i].status);
        if (p_wl[i].status == 0)
            free_n++;
        else if (p_wl[i].status == 1)
        if (kill(p_wl[i].pid, 0) != 0)
        {
            p_wl[i].status=0;
            free_n++;
        }
    }

    //printf("\n");

    if (free_n < N && p_wm->processing_n+M < BACKLOG)
        enlarge_pool();
    else if (free_n > K)
        shrink_pool(M);

    ERROR(pthread_mutex_unlock(&p_wm->mutex), "rwl_unl");

    return 0;
}

int maintain_pool()
{
    for (;;)
    {
        sleep(1);
        check_pool();
    }

    return 0;
}

void server_destroy(int sig)
{
    int i;

    ERROR(pthread_mutex_lock(&p_wm->mutex), "mut_loc");

    for (i=0; i < p_wm->processing_n; i++)
        ERROR(kill(p_wl[i].pid, SIGKILL), "kill");

    for (i=0; i < p_wm->processing_n; i++)
        ERROR(wait(NULL), "wait");

    ERROR(shmdt((void*)p_wl), "shmdt");
    ERROR(shmctl(shmidwl, IPC_RMID, NULL), "shmctl");
    ERROR(pthread_mutex_unlock(&p_wm->mutex), "mut_unl");
    ERROR(pthread_mutex_destroy(&p_wm->mutex), "mut_des");
    ERROR(shmdt((void*)p_wm), "shmdt");
    ERROR(shmctl(shmidm, IPC_RMID, NULL), "schctl");
    ERROR(close(sd), "close");

    _exit(1);
}

int main(int argc, char* argv[])
{
    printf("For exit press Ctrl-c, server port - %d\n", PORT);
    signal(SIGINT, server_destroy);
    server_init();
    ERROR(pthread_mutex_lock(&p_wm->mutex), "mut_lck");
    enlarge_pool();
    ERROR(pthread_mutex_unlock(&p_wm->mutex), "mut_ulc");
    maintain_pool();

    return 0;
}
