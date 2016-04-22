#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>

#include "information.h"
#include <stdlib.h>

char alphabet[] = {
  'a','b','c','d','e','f','g','h'
  ,'i','j','k','l','m','n','o','p','q'
  ,'r','s','t','u','v','w','x','y','z'
};
int semid;

static struct sembuf main_lock = { 0, -1, 0 };
static struct sembuf inv_lock = { 1, -1, 0 };
static struct sembuf rev_lock = { 2, -1, 0 };

static struct sembuf main_unlock = { 0, 1, 0 };
static struct sembuf inv_unlock = { 1, 1, 0 };
static struct sembuf rev_unlock = { 2, 1, 0 };

void *transposer(void __attribute__((unused)) *arg) {
  size_t i;
  char t;
  for (;;) {
    semop(semid, &rev_lock, 1);
    
    for (i = 0; i < sizeof(alphabet) / 2;i++){
      t = alphabet[i];
      alphabet[i] = alphabet[sizeof(alphabet)-1-i];
      alphabet[sizeof(alphabet)-1-i] = t;
    }
    
    semop(semid, &main_unlock, 1);
  }
  return NULL;
}

void *inverter(void __attribute__((unused)) *arg) {
  char* c;
  int t;
  char *str;
  
  for (;;) {
    semop(semid, &inv_lock, 1);
    
    str = &alphabet[0];
    if (str) {
      for (c = str; (t = *c) != 0; ++c)
	if (isalpha(t))
	  *c = (islower(t)?toupper(t):tolower(t));
    }

    semop(semid, &main_unlock, 1);
  }
  return NULL;
}

int main() {
  pthread_t transposer_thread;
  pthread_t inverter_thread;

  size_t i;

  key_t key = 182190;
  //if((key = ftok("5task.c", 0)) < 0) {
  //  perror("ftok");
  //  return 1;
  //}

  if ((semid = semget(key, 3, 0644 | IPC_CREAT)) != 0) {
    perror("semget error");
    return 1;
  }
  
  if (pthread_create(&transposer_thread, NULL, transposer, NULL) != 0) {
    perror("transposer's thread creation failed");
    return 1;
  }
  if (pthread_create(&inverter_thread, NULL, inverter, NULL) < 0) {
    perror("inverter's thread creation failed");
    return 1;
  }
    

  for (;;) {
    semop(semid, &inv_unlock, 1);
    semop(semid, &main_lock, 1);
    for (i=0; i < sizeof(alphabet); i++)
      printf("%c ",alphabet[i]);
    printf("\n");
    sleep(1);
    semop(semid, &rev_unlock, 1);
    semop(semid, &main_lock, 1);
    for (i=0;i<sizeof(alphabet);i++) 
      printf("%c ",alphabet[i]);
    printf("\n");
    sleep(1);
  }
  return 0;
}
