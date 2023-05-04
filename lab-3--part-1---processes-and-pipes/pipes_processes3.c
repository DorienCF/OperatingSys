#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char **argv)
 {
  int pipefd[2];
  int pipefd2[2];

  // error checking  for pipe ... might remove... Nah
  if (pipe(pipefd) == -1 || pipe(pipefd2) == -1)
    {
    fprintf(stderr, "##########\nPipe one or two failed.\n##########");
    return 1;
    }
  // ADMIN: if pipe fail check gtg; close.

  int pid, pid2;

  // required "Cat", "grep (arg)" and "sort" commands.

  char *sort_args[] = {"sort", NULL, NULL};
  char *grep_args[] = {"grep", argv[1], NULL};
  char *cat_args[] = {"cat", "scores", NULL};

  if (argv[1] == NULL)
   {
    printf("You need to enter an argument...\n");
    exit(0);
  }
  // ADMIN: IF gtg, processing EMPTY argv[1]; close. 

  pipe(pipefd);
  pid = fork();

  if (pid == 0) 
  {
    dup2(pipefd[0], 0);
    close(pipefd[1]);
    execvp("grep", grep_args);

  } 
  // ADMIN: if gtg processing grep; close
  else 
  {
    pid2 = fork();
    if (pid2 == 0) 
    {
      dup2(pipefd[1], 1);
      close(pipefd[0]);
      execvp("cat", cat_args);
    }
    // ADMIN: IF gtg processing the cat; close
    dup2(pipefd[1], 1);
    close(pipefd[0]);
    execvp("sort", sort_args);   
  }
    // ADMIN: ELSE gtg processing the sort; close 
    return 1;
} // end of main, gtgt close

/*
 Program returns sorted selction based off of the input argument. HOWEVER it does not want ot properly exit... weird. 
*/