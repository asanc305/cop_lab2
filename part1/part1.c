#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h>

int SharedVariable = 0 ;
pthread_barrier_t barrier ;
pthread_mutex_t lock ;

void *SimpleThread( void * x )
{
  int num, val, which ;

  which = ( int ) x ;

  for( num = 0; num < 20; num++ )
  {
    if (random() > RAND_MAX / 2 )
      usleep(10) ;

    #ifdef PTHREAD_SYNC
      pthread_mutex_lock( &lock ) ;
    #endif

    val = SharedVariable ;
    printf("*** thdread %d sees value %d\n", which, val) ;
    SharedVariable = val + 1 ;

    #ifdef PTHREAD_SYNC
      pthread_mutex_unlock( &lock ) ;
    #endif
  }

  #ifdef PTHREAD_SYNC
    pthread_barrier_wait( &barrier ) ;
  #endif  
  val = SharedVariable ;
  printf("Thread %d sees final value %d\n", which, val) ;
}

int main (int args, char *argv[])
{
  int i, numThreads ;

  if ( args != 2 )
    printf("call <number of threads\n") ;

  numThreads = atoi( argv[1] ) ;
  pthread_t threads[ numThreads ] ;

  pthread_barrier_init( &barrier, NULL, numThreads + 1 ) ;
  pthread_mutex_init( &lock, NULL ) ;

  for( i = 0; i <= numThreads; i++ )
  {
    if( pthread_create( &threads[i], NULL, SimpleThread, ( void* ) i ) != 0 ) 
    {
      perror("Student thread could not be created!\n");
      exit(0);
    }
  }
  while( 1 ) ;

}
