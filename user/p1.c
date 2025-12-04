#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main()
{
  int free = kmemfree();

  printf("Total free memory: %d\n", free);

}
