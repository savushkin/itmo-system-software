#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include <sys/loadavg.h>

struct info_t {
  pid_t server_pid;
  pid_t server_uid;
  pid_t server_gid;
  double uptime;
  double load[3];
};

struct message_t {
  long mtype;
  struct info_t information;
};

struct flag_t {
  long mtype;
  int connected;
};

#define UNIQUE_MAGIC_KEY 182190

#endif
