
#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#define   MAX_COUNT  10

void  ChildProcess(void);     
void  ParentProcess(int);    

void  main() 
{

            // Beginning variables w/  PIDs, iteator and status variuble 
  
  pid_t  PID;
  pid_t PID2;
  int i;
  int status;
            // ######
  for (i = 0; i < 2; i++) 
    {
      PID = fork();             
      if (PID == 0)
        {
          ChildProcess();
          exit(0);
        } //## ADMIN, if statement gtg; close
      } //## ADMIN, for loop gtg; close.

  for (i = 0; i < 2; i++) 
    {
    wait(&status);
    ParentProcess(status);
    } //## ADMIN, for loop gtg; close.
} // ## ADMIN, main method gtg close.

void  ChildProcess() 
  {
      // Beginning variables: iterator, loop limiter(<=30),child &parant PID
  int i;
  srand(getpid());
  int loop_lim = rand()%30;
  pid_t ChildPID = getpid();
  pid_t ParentPID = getppid();
      // ############

  for (i = 0; i < loop_lim; i++) 
  {
    printf("Child PID: %d is going to sleep! \n", ChildPID);
    sleep( rand()%MAX_COUNT );    // sleep is set to a time @ or less than 10.
    printf("Child Pid: %d is awake!\nWhere is my Parent: %d?\n", ChildPID, ParentPID);
  }// ## ADMIN, for loop gtgt; close.
} //## ADMIN ChildProcess method gtg; close

void  ParentProcess(int status)
{
  pid_t ChildPID = getpid();
  wait(&status); printf("Parent Pid: %d has completed!\n", ChildPID);
  printf("*** Parent is done, Child exited with exit status %d ***\n", WEXITSTATUS(status));
}//##ADMIN ParentProcess gtg; close.