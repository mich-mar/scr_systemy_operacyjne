/*****************************************************************************
* FILE: bug3.c
* DESCRIPTION:
*   This "hello world" Pthreads program demonstrates an unsafe (incorrect)
*   way to pass thread arguments at thread creation. Compare with hello_arg1.c.
* AUTHOR: Blaise Barney
* LAST REVISED: 07/16/14
******************************************************************************/
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS     8

void *PrintHello(void *threadid)
{
   long taskid;
   sleep(1);
   taskid = *(long *)threadid;
   printf("Hello from thread %ld\n", taskid);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
pthread_t threads[NUM_THREADS];
int rc;
long t;

for(t=0;t<NUM_THREADS;t++) {
  printf("Creating thread %ld\n", t);

  rc = pthread_create(&threads[t], NULL, PrintHello, (void *) t);
  if (rc) {
    printf("ERROR; return code from pthread_create() is %d\n", rc);
    exit(-1);
    }
   }

pthread_exit(NULL);
}

// Zmienna t jest współdzielona przez wszystkie iteracje pętli, co oznacza, że każdy wątek otrzymuje adres tej samej
// zmiennej (&t). Podczas gdy wątki uruchamiają się asynchronicznie, wartość t może zmieniać się zanim wątki faktycznie
// ją odczytają. To prowadzi do sytuacji, gdzie wątki mogą widzieć inną wartość t niż ta, która była podczas wywołania
// pthread_create.
