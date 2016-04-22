#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>

char alphabet[] = {
  'a','b','c','d','e','f','g','h'
  ,'i','j','k','l','m','n','o','p','q'
  ,'r','s','t','u','v','w','x','y','z'
};

sem_t sem1;
sem_t sem2;
sem_t sem3;
sem_t sem4;

size_t i;

void *transposer(void __attribute__((unused)) *arg) {
  size_t i;
  char t;
  
  for (;;) {
    sem_wait(&sem2);
    for (i = 0; i < sizeof(alphabet) / 2;i++){
      t = alphabet[i];
      alphabet[i] = alphabet[sizeof(alphabet)-1-i];
      alphabet[sizeof(alphabet)-1-i] = t;
    }
    sem_post(&sem4);
  }
  return NULL;
}

void *inverter(void __attribute__((unused)) *arg) {
  char* c;
  int t;
  char *str;
  
  for (;;) {
    sem_wait(&sem1);
    str = &alphabet[0];
    if (str) {
      for (c = str; (t = *c) != 0; ++c)
	if (isalpha(t))
	  *c = (islower(t)?toupper(t):tolower(t));
    }
    sem_post(&sem3);
  }
  return NULL;
}

int main() {
  pthread_t transposer_thread;
  pthread_t inverter_thread;
  
  if (pthread_create(&transposer_thread, NULL, transposer, NULL) != 0) {
    perror("transposer's thread creation failed");
    return 1;
  }
  if (pthread_create(&inverter_thread, NULL, inverter, NULL) != 0) {
    perror("inverter's thread creation failed");
    return 1;
  }
        
  if (sem_init(&sem1, 0, 0) < 0){
    perror("sem_init error");
    return 1;
  }
    
  if (sem_init(&sem2, 0,0) < 0){
    perror("sem_init error");
    return 1;
  }

  if (sem_init(&sem3, 0, 0) < 0){
    perror("sem_init error");
    return 1;
  }
    
  if (sem_init(&sem4, 0,0) < 0){
    perror("sem_init error");
    return 1;
  }
    
  for (;;) {
    sem_post(&sem1);
    sem_wait(&sem3);

    for (i=0; i<sizeof(alphabet); i++)
      printf("%c ",alphabet[i]);

    printf("\n");
    sleep(1);

    sem_post(&sem2);
    sem_wait(&sem4);

    for (i=0;i<sizeof(alphabet);i++)
      printf("%c ",alphabet[i]);

    printf("\n");
    sleep(1);
  }
  return 0;
}
