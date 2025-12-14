#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fcntl.h"
#include "user.h"

void test_basic(void)
{
  int fd;
  char buf[16];

  printf("TEST 1: Basic Symbolic Link... ");

  // 1. Create a target file with secret content
  fd = open("target.txt", O_CREATE | O_RDWR);
  if(fd < 0)
  {
    printf("FAILED: could not create target.txt\n");
    exit(1);
  }
  write(fd, "SUCCESS", 7);
  close(fd);

  // 2. Create a symlink: "mylink" -> "target.txt"
  if(symlink("target.txt", "mylink") < 0)
  {
    printf("FAILED: sys_symlink returned error\n");
    exit(1);
  }

  // 3. Open the LINK. If logic is correct, it should open target.txt
  fd = open("mylink", O_RDONLY);
  if(fd < 0)
  {
    printf("FAILED: open(mylink) returned error\n");
    exit(1);
  }

  // 4. Read content
  memset(buf, 0, sizeof(buf));
  read(fd, buf, sizeof(buf));
  close(fd);

  // 5. Verify
  if(strcmp(buf, "SUCCESS") == 0)
  {
    printf("PASSED\n");
  } 
  else 
  {
    printf("FAILED: read '%s' instead of 'SUCCESS'\n", buf);
  }
}

void test_chain(void)
{
  int fd;
  char buf[16];

  printf("TEST 2: Chained Symbolic Links... ");

  // Create link2 -> mylink (which points to target.txt)
  if(symlink("mylink", "link2") < 0)
  {
    printf("FAILED: could not create link2\n");
    exit(1);
  }

  fd = open("link2", O_RDONLY);
  if(fd < 0)
  {
    printf("FAILED: open(link2) returned error\n");
    exit(1);
  }

  memset(buf, 0, sizeof(buf));
  read(fd, buf, sizeof(buf));
  close(fd);

  if(strcmp(buf, "SUCCESS") == 0)
  {
    printf("PASSED\n");
  } 
  else 
  {
    printf("FAILED: read '%s'\n", buf);
  }
}

void test_cycle(void)
{
  int fd;
  printf("TEST 3: Cyclic Links (Should fail safely)... ");

  // Create Cycle: A -> B and B -> A
  symlink("file_b", "file_a");
  symlink("file_a", "file_b");

  // Try to open. This should trigger your MAXATTEMPTS check.
  fd = open("file_a", O_RDONLY);
  
  if(fd < 0)
  {
    printf("PASSED (Returned -1 as expected)\n");
  } 
  else
  {
    printf("FAILED (Open succeeded? That's bad for a cycle)\n");
    close(fd);
  }
}

int main(int argc, char *argv[])
{
  test_basic();
  test_chain();
  test_cycle();
  exit(0);
}
