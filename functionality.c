#define _GNU_SOURCE
#include "funnies.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/sendfile.h>
void instruction_manual()
{
    syslog(LOG_NOTICE, "Displaying manual page...");
    fprintf(stdout, "This is where functionality will be explained...one day. (lmao) (rofl)");
}
int appendSlash(char* entry_path, int path_len)
{
  if (entry_path[path_len - 1] != '/')
  {
    entry_path[path_len] = '/';
    entry_path[path_len + 1] = '\0';
    return 1;
  }
  return 0;
}
void deleteAll(char* givenPath)
{
    DIR* dir;
    struct dirent* entry;
    char path[1024];
    if (!(dir = opendir(givenPath)))
    {
        syslog(LOG_ERR, "FUN: Could not open directory");
        return;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
        {
            snprintf(path, 1024, "%s/%s", givenPath, entry->d_name);
            remove(path);
        }
    }
    closedir(dir);
}

void copyFiles(char* sourcePath, char* destinationPath)
{
  syslog(LOG_NOTICE, "copyFiles: copying files using sendfile...", sourcePath, destinationPath);
  int sourceFolder;
  int destinationFolder;
  struct stat status;
  off_t offset = 0;

  sourceFolder = open(sourcePath, O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH);
  if (sourceFolder == -1)
  {
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }

  fstat(sourceFolder, &status);
  destinationFolder = open(destinationPath, O_WRONLY | O_CREAT, status.st_mode);


  if (destinationFolder == -1)
  {
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  if (sendfile(destinationFolder, sourceFolder, &offset, status.st_size) == -1)
  {
    perror("Sendfile error:");
    exit(EXIT_FAILURE);
  }

  close(sourceFolder);
  close(destinationFolder);
}

//create a function that browses linux directories and copies files to another directory
void browseDirectory(char* sourcePath, char* destinationPath, int isRecursive)
{
  syslog(LOG_NOTICE, "BROWSER: Browse directory: %s", sourcePath);
  DIR* dir;
  struct dirent* entry;
  char path[1024];
  if (!(dir = opendir(sourcePath)))
  {
    syslog(LOG_ERR, "BROWSER: Could not open directory");
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(path, 1024, "%s/%s", sourcePath, entry->d_name);
      if (isRecursive)
      {
        if (entry->d_type == DT_DIR)
        {
          browseDirectory(path, destinationPath, isRecursive);
        }
        else
        {
          copyFiles(path, destinationPath);
        }
      }
      else
      {
        if (entry->d_type == DT_DIR)
        {
          syslog(LOG_NOTICE, "FUN: Skipping directory: %s", path);
        }
        else
        {
          copyFiles(path, destinationPath);
        }
      }
    }
  }
  closedir(dir);
}







































































