#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "information.h"

int main()
{
  struct message_t message;

  message.information.server_pid = getpid();
  message.information.server_uid = getuid();
  message.information.server_gid = getgid();
  message.mtype=1;

  time_t start_time;
  start_time=time(NULL);
    
  key_t key;
  if((key = ftok("2server", 0)) < 0) {
    perror("ftok");
    return 1;
  }
    
  int msgid;
  if ((msgid = msgget(key, 0644 | IPC_CREAT)) <0) {
    perror("msgget error");
    return 1;
  }

  for (;;) {
    time_t current_time=time(NULL);
    message.information.uptime = difftime(current_time,start_time);
    getloadavg(message.information.load, 3);
    msgsnd(msgid, &message , sizeof(message.information), 0);
    sleep(1);
  }
  return 0;
}
