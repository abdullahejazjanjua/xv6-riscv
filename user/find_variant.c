#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fs.h"
#include "../kernel/fcntl.h"
#include "../kernel/param.h"
#include "user.h"


char* fmtname(char *path)
{
  char *p;
  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;
  return p;

}


void find(char *path, char *filename, char *files[], int *i, int use_exec, char *exec_args[], int exec_argc)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, O_RDONLY)) < 0)
  {
    fprintf(2, "ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0)
  {
    fprintf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type)
  {
    case T_DEVICE:
    case T_FILE:
    {
      if (!strcmp(fmtname(path), filename))
      {
        if (use_exec)
        {
          exec_args[exec_argc] = path;
          exec_args[exec_argc + 1] = 0;
          int c = fork();
          if (c == 0)
          {
            exec(exec_args[0], exec_args);
            fprintf(2, "find: exec %s failed\n", exec_args[0]);
            exit(1);
          }
          else 
          {
            wait(0);
          }
        }
        else
        {
          int len = strlen(path) + 1;                    
          files[*i] = (char*)malloc(len);
          if (files[*i] == 0) 
          {
            fprintf(2, "find: malloc failed\n");
            break; 
          }
          memmove(files[*i], path, len);             
          (*i)++;
        }
      }
      break;
    }
    case T_DIR:
    { 
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
        printf("ls: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/';
      while(read(fd, &de, sizeof(de)) == sizeof(de))
      {
        if(de.inum == 0)
          continue;
        
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        
        if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
        
        find(buf, filename, files, i, use_exec, exec_args, exec_argc);
      }
      break;

    }
  }
  close(fd);
}




int main(int argc, char *argv[])
{
  if(argc < 3)
  {
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }

  char *exec_args[MAXARG];
  int exec_argc = 0;
  int use_exec = 0;
  char **p;
  
  if (argc >= 5 && !strcmp(argv[3], "-exec"))
  {
    p = argv + 4; // move after -exec
    use_exec = 1;
    while(*p && exec_argc < MAXARG - 1)
    {
      exec_args[exec_argc++] = *p;
      p++;
    }
    exec_args[exec_argc] = 0;
  }
 
  
  char *found_files[100];
  int file_index = 0;
      
  find(argv[1], argv[2], found_files, &file_index, use_exec, exec_args, exec_argc);
  
  for (int j = 0; j < file_index; j++) 
  {
      fprintf(1, "%s\n", found_files[j]);
      free(found_files[j]); 
  }
  
  exit(0);
}
