#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h>
#include "part1.h"

int SharedVariable = 0 ;
pthread_barrier_t barrier ;

void *SimpleThread( void * x )
{
  int num, val, which ;

  which = ( int ) x ;

  for( num = 0; num < 20; num++ )
  {
    if (random() > RAND_MAX / 2 )
      usleep(10) ;

    val = SharedVariable ;
    printf("*** thread %d sees value %d\n", which, val) ;
    SharedVariable = val + 1 ;
  }
  
  #ifdef PTHREAD_SYNC
    pthread_barrier_wait( &barrier ) ;
  #endif
  val = SharedVariable ;
  printf("Thread %d sees final value %d\n", which, val) ;
  
}

int isNumeric (const char * s)
{
  if (s == NULL || *s == '\0')
    return 0;
  char * p;
  strtod (s, &p);
  return *p == '\0';
}

int main (int args, char *argv[])
{
  int i, numThreads ;

  if ( args != 2 || isNumeric( argv[1] ) == 0 )
  {
	printf("call <number of threads>\n") ;
	return -1 ;	  
  }
    
  numThreads = atoi( argv[1] ) ;
  pthread_t threads[ numThreads ] ;

  pthread_barrier_init( &barrier, NULL, numThreads ) ;

  for( i = 1; i <= numThreads; i++ )
  {
    if( pthread_create( &threads[i], NULL, SimpleThread, ( void* ) i ) != 0 ) 
    {
      perror("Student thread could not be created!\n");
      exit(0);
    }
  }
  syscall(__NR_sanchez, 0) ;
  while( 1 ) ;

}
