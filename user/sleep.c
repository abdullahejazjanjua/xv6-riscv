#include "../kernel/types.h"
#include "user.h"


int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("USAGE: sleep <ticks>");
  }
  int ticks = atoi(argv[1]);

  pause(ticks);
}
