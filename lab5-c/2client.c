#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "information.h"

int main() {
  key_t key;
  if((key = ftok("2server", 0)) < 0) {
    perror("ftok");
    return 1;
  }
  int msqid;
  if ((msqid = msgget(key, 0644)) <0) {
    perror("msgget error");
    return 1;
  }

  struct message_t message;
  for(;;) {
    msgrcv(msqid, &message, sizeof(message.information), 1, 0);

    printf("Server's PID: %ld\n",message.information.server_pid);
    printf("Server's UID: %ld\n",message.information.server_uid);
    printf("Server's GID: %ld\n",message.information.server_gid);
    printf("Server is up: %.0f sec\n",message.information.uptime);
    printf("Load averages: %.2f , %.2f , %.2f\n\n", message.information.load[0],message.information.load[1],message.information.load[2]);
    printf("\033[6A\033[K");
  }
  return 0;
}
