#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "user.h"

int main(int argc, char *argv[])
{
  char buf[32];
  char *sep =  " -\r\t\n./,";
  int fd = open(argv[1], O_RDONLY);
  
  int idx = 0;
  int valid = 1; 
  char c;

  while(read(fd, &c, 1) != 0)
  {
    if (strchr(sep, c)) 
    {
      if (idx > 0 && valid)
      {
        buf[idx] = '\0';
        printf("%s\n", buf);
      }
        
      idx = 0; 
      valid = 1;
    }
    else if (c >= '0' && c <= '9')
    {
      if (valid)
      {
        buf[idx] = c;
        idx++;
      }
    }
    else 
    {
      valid = 0; 
      idx = 0;   
    }
  }
}
