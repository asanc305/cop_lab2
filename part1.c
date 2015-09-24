#include <stdlib.h> 
#include <stdio.h> 
#include <pthread.h>

int SharedVariable = 0 ;

void SimpleThread( void * which )
{
  int num, val ;

  for( num = 0; num < 20; num++ )
  {
    if (random() > RAND_MAX / 2 )
      usleep(10) ;

    val = SharedVariable ;
    printf("*** thdread %d sees value %d\n", which, val) ;
    SharedVariable = val + 1 ;
  }

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

  for( i = 0; i <= numThreads; i++ )
  {
    if( pthread_create( &threads[i], NULL, SimpleThread, i ) != 0 ) 
    {
      perror("Student thread could not be created!\n");
      exit(0);
    }
  }
  printf("%i\n", numThreads) ;

}
