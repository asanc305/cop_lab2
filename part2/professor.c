#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include "professor.h"

pthread_t threads[100] ;

struct timeval t1, t2 ;

int students = 0 ;
int available = 0 ;
int qNumber = 0 ;
int disId = 0 ; 
int capacity = 0 ;
int eid = 0 ;

pthread_cond_t noStudents ;
pthread_cond_t notAvailable ;
pthread_cond_t noQuestion ;
pthread_cond_t noAnswer ;
pthread_cond_t fullCapacity ;

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
  printf("Professor starts to answer question for student %i.\n", disId) ;

  gettimeofday(&t1, NULL) ;
  
  usleep(100000) ;

}

void AnswerDone()
{

  gettimeofday(&t2, NULL) ; 
  pthread_mutex_lock(&answerLock) ;
  printf("Professor is done with answer for student %i.\n", disId) ;
  qNumber = 0 ;
  pthread_mutex_unlock(&answerLock) ;
  pthread_cond_signal(&noAnswer) ;

}

void * Student_Start(void * s) 
{
  int id = (*(Studentt *) s).id ;
  int noq = (*(Studentt *) s).noq ;  

  pthread_mutex_lock(&lock) ;

  pthread_mutex_lock( &studentLock ) ;
  while( students >= capacity ) pthread_cond_wait( &fullCapacity, &studentLock ) ;
  students++ ;
  eid = id ;
  EnterOffice() ;

  int q = 1 ;
  while(q <= noq) 
  {
    pthread_mutex_lock(&studentLock) ;
    while (!available) pthread_cond_wait(&notAvailable, &studentLock) ; 
    pthread_mutex_unlock(&studentLock) ;
	
	pthread_mutex_lock(&questionLock) ;
	disId = id ;
	qNumber = q ;
    QuestionStart() ;
	pthread_mutex_unlock(&questionLock) ;
	pthread_cond_signal(&noQuestion) ;

    QuestionDone() ;
    
    q++ ;

    pthread_mutex_unlock(&lock) ;
  }

  pthread_mutex_lock( &studentLock ) ;
  eid = id ;
  LeaveOffice( ) ;

  pthread_exit(NULL);

  return NULL;

}

void EnterOffice()
{
  printf("Student %i shows up at the office.\n", eid) ;
  
  pthread_cond_signal( &noStudents ) ;
  pthread_mutex_unlock( &studentLock ) ;
  
}

void LeaveOffice( )
{
  students-- ;
  
  printf("Student %i leaves office.\n", eid) ;
  
  pthread_cond_signal( &fullCapacity ) ;
  
  pthread_mutex_unlock(&studentLock) ;

  
}

void QuestionStart()
{
  printf("Student %i asks a question.\n", disId) ;
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

int isNumeric (const char * s)
{
  if (s == NULL || *s == '\0')
    return 0;
  char * p;
  strtod (s, &p);
  return *p == '\0';
}

int main (int argc, char *argv []) 
{
  int i, j ;
  if (argc != 3 || isNumeric( argv[1] ) == 0 || isNumeric( argv[2] ) == 0 )
  {
    printf("Usage: %s <number_of_students> <capacity>\n", argv[0]) ;
	return -1 ;
  }
  
  Professor();
  
  j = atoi( argv[1] ) ;
  capacity = atoi( argv[2] ) ;
  
  for( i = 0; i < j; i++)
  {
    Student( i ) ;
  } 
  while (1);
}
