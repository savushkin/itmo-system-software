//
// Created by ivan on 04.06.15.
//
#include <sys/types.h>
#include <pthread.h>

#define BACKLOG 100
#define M 10

extern errno;

typedef struct
{
    int status;
    pid_t pid;
} processing_info;

typedef struct
{
    pthread_mutex_t mutex;
    int processing_n;
} processing_mut;

extern int sd;
extern int processing_n;
extern processing_info* p_wl;
extern processing_mut* p_wm;
