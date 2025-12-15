#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "user.h"
#include "../kernel/fcntl.h"
#include "../kernel/fs.h" 

int main(int argc, char *argv[])
{
  int fd;
  int i;
  char buf[BSIZE];
  
  memset(buf, 'A', sizeof(buf));

  printf("Starting stress test...\n");
  fd = open("test_file", O_CREATE | O_RDWR);
  if(fd < 0)
  {
    printf("Error: cannot open test_file\n");
    exit(1);
  }

  // int target_blocks = NDIRECT + NINDIRECT + NDINDIRECT;  // fails
  int target_blocks = 65803; // Works
  
  for(i = 0; i < target_blocks; i++)
  {
    if(write(fd, buf, sizeof(buf)) != sizeof(buf))
    {
      printf("CRITICAL: Write failed at block %d\n", i);
      exit(1);
    }
    
    if (i == (NDIRECT + NINDIRECT - 1)) 
    {
       printf("Milestone: Finished Singly-Indirect blocks (Block %d)\n", i);
    }
    if (i == (NDIRECT + NINDIRECT)) 
    {
       printf("Milestone: SUCCESSFULLY wrote first Doubly-Indirect block (Block %d)!\n", i);
    }
  }

  printf("Test Passed: Wrote %d blocks.\n", target_blocks);
  close(fd);
  exit(0);
}
