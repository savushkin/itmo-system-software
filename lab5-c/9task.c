#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include "information.h"

struct info_t information;

static void handler(int signo) {
  if(signo == 1) printf("pid: %ld\n\n",information.server_pid);
  if(signo == 2) printf("uid: %ld\n\n",information.server_uid);
  if(signo == 15) printf("gid: %ld\n\n",information.server_gid);
  if(signo == 16) printf("uptime: %.0f sec\n\n",information.uptime);
  if(signo == 17) printf("load: %.2f , %.2f , %.2f\n\n", information.load[0], information.load[1],information.load[2]);
}
 
int main() {
  time_t start_time;

  information.server_pid=getpid();
  information.server_uid=getuid();
  information.server_gid=getgid();
  start_time=time(NULL);
    
  struct sigaction act;
  memset (&act, '\0', sizeof(act));
  act.sa_handler = &handler;

  if (sigaction(SIGINT, &act, NULL) < 0)
    perror("SIGINT handling failed");

  if (sigaction(SIGHUP, &act, NULL) < 0)
    perror("SIGHUP handling failed");

  if (sigaction(SIGUSR1, &act, NULL) < 0)
    perror("SIGUSR1 handling failed");

  if (sigaction(SIGUSR2, &act, NULL) < 0)
    perror("SIGUSR2 handling failed");

  if (sigaction(SIGTERM, &act, NULL) < 0)
    perror("SIGTERM handling failed");

                
  printf("pid: %ld\n\n",information.server_pid);
  for (;;) {
    time_t current_time=time(NULL);
    information.uptime=difftime(current_time,start_time);
    getloadavg(information.load, 3);
    sleep(1);
  }
  return 0;
}
