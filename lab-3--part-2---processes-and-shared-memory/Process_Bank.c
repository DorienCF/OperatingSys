#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/types.h>
#include  <sys/ipc.h>
#include  <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>


void  ParentProcess(int []);
void  ChildProcess(int []);

int  main(int  argc, char *argv[])
{
     int    ShmID;
     int    *ShmPTR;
     pid_t  pid;
     int    status;


     ShmID = shmget(IPC_PRIVATE, 4*sizeof(int), IPC_CREAT | 0666);
     if (ShmID < 0) 
     {
          printf("*** shmget error (server) ***\n");
          exit(1);
     }
     //Given
     printf("Server has received a shared memory of four integers...\n");

     ShmPTR = (int *) shmat(ShmID, NULL, 0);
     if (*ShmPTR == -1)
     {
          printf("*** shmat error (server) ***\n");
          exit(1);
     }
     // Given
     printf("Server has attached the shared memory...\n");

     // Just this is where the banking program will start ... 

     ShmPTR[0] = 0; // Global Bank Account
     ShmPTR[1] = 0; // Turn 
     
     printf("Server is about to fork a child process...\n");
     printf("Orig Bank Account = $%d\n",ShmPTR[0]);
     pid = fork();

     if (pid < 0) 
     {
          printf("*** fork error (server) ***\n");
          exit(1);
     }
     // Given
     else if (pid == 0) 
     {
          ChildProcess(ShmPTR);
          exit(0);
     }//Given
     else
     {
          ParentProcesses(ShmPTR);        
     }

     wait(&status);
     printf("Server has detected the completion of its child...\n");
     shmdt((void *) ShmPTR);
     printf("Server has detached its shared memory...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server has removed its shared memory...\n");
     printf("Server exits...\n");
     exit(0);
}


void ParentProcesses(int SharedMem[])
{

  int Account, Balance, i;
  srand(getpid());

  for (i = 0; i < 25; i++) // printing 25 times.
  {
    sleep(rand()%6); // TASK 01
    while (SharedMem[1] != 0); // while turn != 0 TASK 02
    Account = SharedMem[0]; // TASK 03 // CHECK TO MOVE IN OR OUT OF WHILE LOOP ( ) 
    if (Account <= 100){
      Balance= rand()%101;// TASK 04: DEPOSIT MONEY | DEPOSIT MONEY 01
      if (Balance% 2 == 0){ // CHEXCKING IF EVEN
        Account += Balance; 
        printf("Dear old Dad: Deposits $%d / Balance = $%d\n", Balance, Account); // DEPOSIT MONEY 02
       
      }
      else
      {
        printf("Dear old Dad: Doesn't have any money to give\n");
      }
      SharedMem[0] = Account;
      
    }
    else
    {
      printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", Account); // TASK 04: PRINT
    }
    SharedMem[1] = 1;

    
    
  }

}

void ChildProcess(int  SharedMem[])
{
  int account, Balance,i;
  srand(getpid());

  for (i = 0; i < 25; i++)
  {
    sleep(rand()%6); // TASK 01
    while (SharedMem[1] != 1); // TASK 03
    account = SharedMem[0];// TASK 02
    
    Balance = rand()%51; // TASK 04 - GENERATING A BALANCE NEEDED FROM 0-50
    printf("Poor Student needs $%d\n", Balance); // TASK 04- PRINT STATEMENT
    if (Balance <= account) // TASK 05
      {
      account -= Balance;
        printf("Poor Student: Withdraws $%d / Balance = $%d\n", Balance, account);  
      } 
    else                    // TASK 06
     {
       printf("Poor Student: Not Enough Cash ($%d)\n", account );
      }
      SharedMem[0] = account; //TASK 07
      SharedMem[1] = 0;       // TASK 08
    }
} 
// ADMIN: CHILD PROCESS GTG; CLOSE

    
   


