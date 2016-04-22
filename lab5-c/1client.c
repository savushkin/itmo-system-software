#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "information.h"

int main() {
  struct info_t *info_ptr;

  key_t key;
  if((key = ftok("1server", 0)) < 0) {
    perror("ftok");
    return 1;
  }
    
  int shmid;
  if ((shmid = shmget(key, sizeof(struct info_t), 0644)) <0) {
    perror("shmget failed");
    return 1;
  }
    
  info_ptr= shmat(shmid, (void *)0, 0);
  if (info_ptr == (struct info_t *)(-1)) {
    perror("shmat failed");
    return 1;
  }
  for(;;) {
    printf("Server's PID: %ld\n",info_ptr->server_pid);
    printf("Server's UID: %ld\n",info_ptr->server_uid);
    printf("Server's GID: %ld\n",info_ptr->server_gid);
    printf("Server is up: %.0f sec\n",info_ptr->uptime);
    printf("Load averages: %.2f , %.2f , %.2f\n\n", info_ptr->load[0],info_ptr->load[1],info_ptr->load[2]);
    printf("\033[6A\033[K");
    usleep(500000);
  }
  if (shmdt( (char*) info_ptr)<0) {
    perror("shmdt failed");
    return 1;
  }
  return 0;
}
