#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <unistd.h>
#include <time.h>

#include "information.h"
#define SERVER_PATH     "/tmp/s182190"
#define TIMEOUT 1000

int  main() {
  int     sd;
  int     sd2;
  int     rc;
  struct sockaddr_un serveraddr;

  printf("%ld\n", getpid());
  sd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("socket() failed");
    return 1;
  }

  unlink(SERVER_PATH);
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sun_family = AF_UNIX;
  strcpy(serveraddr.sun_path, SERVER_PATH);

  rc = bind(sd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_un));
  if (rc < 0) {
    perror("bind() failed");
    return 1;
  }

  rc = listen(sd, 10);
  if (rc< 0) {
    perror("listen() failed");
    return 1;
  }

  struct info_t information;
  information.server_pid=getpid();
  information.server_uid=getuid();
  information.server_gid=getgid();
  time_t start_time=time(NULL);

  struct pollfd fds[1];    
  memset(fds, 0 , sizeof(fds));
  fds[0].fd = sd;
  fds[0].events = POLLIN;

  for (;;) { 
    rc = poll(fds, 1, TIMEOUT);

    if (rc < 0) {
      perror("poll failed");
      return 1;
    } else if ( rc == 0 ) {
      fprintf(stderr, "%s", "timeout exceeded\n");
    } else { 
      sd2 = accept(sd, NULL, NULL);
      if (sd2 < 0) {
	perror("accept() failed");
	break;
      }

      rc = send(sd2, &information, sizeof(information), 0);

      if (rc < 0) {
	perror("send() failed");
	break;
      }

      close(sd2);
    }

    time_t current_time=time(NULL);
    information.uptime=difftime(current_time,start_time);
    getloadavg(information.load, 3);
    sleep(1);
  }

  close(sd);

  unlink(SERVER_PATH);
}
