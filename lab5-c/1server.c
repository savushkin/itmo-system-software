#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "information.h"

int shmid;

int main() {
  struct info_t information;
  struct info_t *info_ptr;
  time_t start_time;

  information.server_pid=getpid();
  information.server_uid=getuid();
  information.server_gid=getgid();
  start_time=time(NULL);
    
  key_t key;
  if((key = ftok("1server", 0)) < 0) {
    perror("ftok");
    return 1;
  }
    
  if ((shmid = shmget(key, sizeof(struct info_t), 0644 | IPC_CREAT))< 0) {
    perror("shmget failed");
    return 1;
  }

  info_ptr = shmat(shmid, (void *)0, 0);
  if (info_ptr == (struct info_t *)(-1)) {
    perror("shmat failed");
    return 1;
  }
    
  for (;;) {
    time_t current_time=time(NULL);
    information.uptime=difftime(current_time,start_time);
    getloadavg(information.load, 3);
    memcpy(info_ptr, &information, sizeof(struct info_t));
    sleep(1);
  }
  return 0;
}
