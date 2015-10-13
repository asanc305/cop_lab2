#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "professor.h"

pthread_t threads[100] ;

struct timeval t1, t2 ;

int students = 0 ;
int available = 0 ;
int qNumber = 0 ;
int disId = 0 ; 
int capacity = 0 ;

pthread_cond_t noStudents ;
pthread_cond_t notAvailable ;
pthread_cond_t noQuestion ;
pthread_cond_t noAnswer ;
pthread_cond_t fullCapacity ;

//pthread_mutex_t capacityLock ;
pthread_mutex_t studentLock ;
pthread_mutex_t questionLock ;
pthread_mutex_t answerLock ;
pthread_mutex_t lock ;

void init()
{

  pthread_mutex_init(&studentLock, NULL) ;
  pthread_mutex_init(&questionLock, NULL) ;
  pthread_mutex_init(&answerLock, NULL) ;
  pthread_mutex_init(&lock, NULL) ;
  //pthread_mutex_init(&capacityLock, NULL) ;

  pthread_cond_init(&noStudents, NULL) ;
  pthread_cond_init(&notAvailable, NULL) ;
  pthread_cond_init(&noQuestion, NULL) ;
  pthread_cond_init(&noAnswer, NULL) ;
  pthread_cond_init(&fullCapacity, NULL) ;

}

void * Professor_Start(void * p) 
{
  while(1) 
  {
    pthread_mutex_lock(&studentLock) ;

    if (students == 0)
      pthread_cond_wait(&noStudents, &studentLock) ;

    available = 1 ;
    pthread_mutex_unlock(&studentLock) ;
    pthread_cond_signal(&notAvailable) ;
	
    pthread_mutex_lock(&questionLock) ;
    while (qNumber == 0)
    {

      pthread_cond_wait(&noQuestion, &questionLock) ;

    }
    pthread_mutex_unlock(&questionLock) ;

    pthread_mutex_lock(&studentLock) ;
    available = 0 ;
    pthread_mutex_unlock(&studentLock) ;

    AnswerStart() ;

    AnswerDone() ;
    sleep(1) ;

  }

  pthread_exit(NULL);

  return NULL;
  
}

void AnswerStart()
{

  pthread_mutex_unlock(&questionLock) ;
  printf("Professor starts to answer question # %i for student %i.\n", qNumber, disId) ;

  gettimeofday(&t1, NULL) ;
  
  usleep(100000) ;

}

void AnswerDone()
{

  gettimeofday(&t2, NULL) ; 
  pthread_mutex_lock(&answerLock) ;
  printf("Professor is done with answering question # %i for student %i.\n", qNumber, disId) ;
  qNumber = 0 ;
  pthread_mutex_unlock(&answerLock) ;
  pthread_cond_signal(&noAnswer) ;

}

void * Student_Start(void * s) 
{
  int id = (*(Studentt *) s).id ;
  int noq = (*(Studentt *) s).noq ;  

  pthread_mutex_lock(&lock) ;

  EnterOffice( id, noq ) ;

  int q = 1 ;
  while(q <= noq) 
  {
    pthread_mutex_lock(&studentLock) ;
    while (!available) pthread_cond_wait(&notAvailable, &studentLock) ; 
    pthread_mutex_unlock(&studentLock) ;
    QuestionStart(id, q) ;

    QuestionDone() ;
    
    q++ ;

    pthread_mutex_unlock(&lock) ;
  }

  LeaveOffice( id ) ;

  pthread_exit(NULL);

  return NULL;

}

void EnterOffice( int id, int q )
{
  pthread_mutex_lock( &studentLock ) ;

  while( students >= capacity ) pthread_cond_wait( &fullCapacity, &studentLock ) ;
  students++ ;

  pthread_cond_signal( &noStudents ) ;
  pthread_mutex_unlock( &studentLock ) ;
  printf("Student %i shows up at the office.\n", id, q) ;
}

void LeaveOffice( int id )
{
  pthread_mutex_lock( &studentLock ) ;

  students-- ;
  
  pthread_cond_signal( &fullCapacity ) ;
  
  pthread_mutex_unlock(&studentLock) ;

  printf("Student %i leaves office.\n", id) ;
}

void QuestionStart(int id, int q)
{
  pthread_mutex_lock(&questionLock) ;
  disId = id ;
  qNumber = q ;
  printf("Student %i asks question # %i.\n", id, q) ;
  pthread_mutex_unlock(&questionLock) ;
  pthread_cond_signal(&noQuestion) ;
}

void QuestionDone()
{
  
  pthread_cond_wait(&noAnswer, &answerLock) ;
  printf("Student %i is satisfied\n", disId) ;
  pthread_mutex_unlock(&answerLock) ;

}

void Student(int id) 
{

  Studentt * st ;

  st = (Studentt *) malloc (sizeof (Studentt)) ;

  st -> id = id ;
  st -> noq = ( id % 4 ) + 1  ;

  if(pthread_create(&threads[id], NULL, Student_Start, st)!=0) 
  {
    perror("Student thread could not be created!\n");
    exit(0);
  }

}

void Professor() 
{

  init() ;

  pthread_t p ;

  if(pthread_create(&p, NULL, Professor_Start, NULL)!=0) 
  {
    perror("Professor thread could not be created!\n");
    exit(0);
  }

}

int main (int argc, char *argv []) 
{
  int i, j ;
  Professor();
  
  j = atoi( argv[1] ) ;
  capacity = atoi( argv[2] ) ;
  
  for( i = 0; i < j; i++)
  {
    Student( i ) ;
  } 
  while (1);
   
}
