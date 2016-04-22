#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "information.h"

#define SERVER_PATH     "/tmp/s182190"

int main() {
  int    sd, rc;
  struct sockaddr_un serveraddr;

  sd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sd < 0) {
    perror("socket() failed");
    return 1;
  }

  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sun_family = AF_UNIX;
  strcpy(serveraddr.sun_path, SERVER_PATH);

  rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr_un) );
  if (rc < 0) {
    perror("connect() failed");
    return 1;
  }

  struct info_t information;
  rc = recv(sd, &information, sizeof(information), 0);

  printf("Server's PID: %ld\n",information.server_pid);
  printf("Server's UID: %ld\n",information.server_uid);
  printf("Server's GID: %ld\n",information.server_gid);
  printf("Server is up: %.0f sec\n",information.uptime);
  printf("Load averages: %.2f , %.2f , %.2f\n\n", information.load[0],information.load[1],information.load[2]);      
  close(sd);
}
