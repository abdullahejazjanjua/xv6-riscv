#include "../kernel/types.h"
#include "../kernel/stat.h"
#include "../kernel/fs.h"
#include "../kernel/fcntl.h"
#include "user.h"


char* fmtname(char *path)
{
  char *p;
  for(p=path+strlen(path); p >= path && *p != '/'; p--);
  p++;
  return p;

}


void find(char *path, char *filename, char *files[], int *i)
{
  char buf[512], *p;
  int fd;
  struct dirent de; // holds one directory entry
  struct stat st;
  
  if((fd = open(path, O_RDONLY)) < 0) // open a file as file  
                                      //        OR 
                                      // Directory as a file that contains directory entries dirent
                                      // Run in terminal
                                      // > cat b (directory)
                                      // .
                                      // ..
                                      // a>
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
      break;
    }
    case T_DIR:
    { 
      if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
      {
        printf("ls: path too long\n");
        break;
      }
      strcpy(buf, path);
      p = buf + strlen(buf);
      *p++ = '/'; // Construct path 
                  // p = ./
      while(read(fd, &de, sizeof(de)) == sizeof(de)) // read a directory entry from fd
      {
        if(de.inum == 0)
          continue;
        
        memmove(p, de.name, DIRSIZ); // Add filename to path
                                    // ./filenmae
        p[DIRSIZ] = 0;
        
        if(!strcmp(de.name, ".") || !strcmp(de.name, ".."))
            continue;
        
        find(buf, filename, files, i);
      }
      break;

    }
  }
  close(fd);
  return; // Go back to previous recursive call
}




int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    fprintf(2, "Usage: find <path> <filename>\n");
    exit(1);
  }
  
    char *found_files[100];
    int file_index = 0;
      
    find(argv[1], argv[2], found_files, &file_index);
  
    for (int j = 0; j < file_index; j++) 
    {
        fprintf(1, "%s\n", found_files[j]);
        free(found_files[j]); 
    }
  
    exit(0);
}
