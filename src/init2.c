#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

pid_t g_childPid;
char** g_childArgs = { 0 };

void signal_handler(int signum) {
  // Forward the signal to the child process
  if ( g_childPid > 0 ) 
  {
    kill(g_childPid, signum);
  }
}

int main(int argc, char *argv[]) 
{
  if ( argc < 2 )
  {
    fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
    return EXIT_FAILURE;
  }

  // Set up signal handling
  struct sigaction sa = { 0 };
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;

  // Register the handler for common signals (e.g., SIGTERM, SIGINT)
  if ( sigaction(SIGTERM, &sa, NULL) == -1 )
  {
    perror("sigaction SIGTERM");
    return EXIT_FAILURE;
  }
  if ( sigaction(SIGINT, &sa, NULL) == -1 )
  {
    perror("sigaction SIGINT");
    return EXIT_FAILURE;
  }

  g_childArgs = (char**)malloc(argc * sizeof(char*));
  if ( ! g_childArgs )
  {
    fprintf(stderr, "Unable to allocate memory for child arguments\n");
    return EXIT_FAILURE;
  }
  g_childArgs[argc - 1] = NULL; 
  g_childArgs[0] = argv[1];
  for ( int i = 2; i < argc; ++i )
  {
    g_childArgs[i - 1] = argv[i];
    // If child argument starts with double underscore then it's
    // the name of an env variable that needs to be substituted.
    if ( strlen(argv[i]) > 2 && argv[i][0] == '_' && argv[i][1] == '_' )
    {
      g_childArgs[i - 1] = getenv(&argv[i][2]);
      if ( ! g_childArgs[i - 1] )
      {
        fprintf(stderr, "Environment variable %s does not exist\n", &argv[i][2]);
        return EXIT_FAILURE;
      }
    }
  }
  int i = 0;
  for ( char** a = &g_childArgs[0]; *a; ++a, ++i )
  {
    printf("Arg[%d] = %s\n", i, *a);
  }

  // Spawn a child process
  g_childPid = fork();

  if ( g_childPid == -1 ) 
  {
    perror("fork");
    return EXIT_FAILURE;
  }

  if ( g_childPid == 0 ) 
  {
    // Child process
    // Execute the command with its parameters
    execvp(g_childArgs[0], g_childArgs);

    free(g_childArgs);
    
    // If execvp fails
    perror("execvp");
    return EXIT_FAILURE;
  } 
  else 
  {
    // Parent process

    // Wait for the child to exit
    int status = 0;
    waitpid(g_childPid, &status, 0);

    if ( WIFEXITED(status) ) 
    {
      printf("Child process exited with status %d\n", WEXITSTATUS(status));
    } 
    else if ( WIFSIGNALED(status) ) 
    {
      printf("Child process terminated by signal %d\n", WTERMSIG(status));
    }
  }

  return 0;
}
