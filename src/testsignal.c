#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static void handler(int signal)
{
  fprintf(stderr, "caught %d\n", signal);
  exit(0);
}

int main(void)
{
  struct sigaction sa = { 0 };
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = &handler;
  if ( sigaction(SIGTERM, &sa, NULL) == -1 )
  {
    perror("sigaction");
    return 1;
  }

  while(1)
  {
    sleep(1);
  }
}
