#include "../kernel/types.h"
#include "../kernel/fcntl.h"
#include "user.h"

void  memdump(char *fmt, char *data)
{
  
  for (; *fmt; fmt++)
  {
      switch (*fmt)
      {
          case 'i':
          {
              int val = *(int*)data;
              printf("%d\n", val);
              data += sizeof(int);
              break;
          }
          case 'p':
          {
              long long val = *(long long*)data;
              printf("0x%llx\n", val);
              data += sizeof(long long);
              break;
          }
          case 'h':
          {
              short val = *(short*)data;
              printf("%d\n", val);
              data += sizeof(short);
              break;
          }
          case 'c':
          {
              char val =  *data;
              printf("%c\n", val);
              data += sizeof(char);
              break;
          }
          case 's':
          {
              char *val = *((char**)data);
              printf("%s\n", val);
              data += sizeof(char*);
              break;
          }
          case 'S':
          {
              printf("%s\n", data);
              return;
          }
          default:
              printf("Invalid option: %c\n", *fmt);
      }
  }
}


int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    printf("USAGE: memdump <specifier> <data>");
    exit(1);
  }
  
  char *data = argv[2];
  char *fmt = argv[1];
  
  printf("data: %s\n", data);
  printf("fmt: %s\n", fmt);
  
  memdump(fmt, data);
  
  
}
