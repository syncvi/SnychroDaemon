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
    char *filename = "manual.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }
    char ch;
    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);
    fclose(fp);
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

time_t timeGetter(char *time)
{
    struct stat _time;
    if(stat(time, &_time) == -1)
    {
        syslog(LOG_ERR, "Sth gone wrong with setting time!!!", time);
    }
    return _time.st_mtime;
}


void compareFiles(char* sourcePath, char* destinationPath)
{
  syslog(LOG_NOTICE, "COMPARER: Comparing files: %s", sourcePath);
  DIR* dir;
  struct dirent* entry;
  char path[1024];
  if (!(dir = opendir(sourcePath)))
  {
    syslog(LOG_ERR, "COMPARER: Could not open directory");
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(path, 1024, "%s/%s", sourcePath, entry->d_name);
      if (entry->d_type == DT_DIR)
      {
        compareFiles(path, destinationPath);
      }
      else
      {
        if (entry->d_type == DT_REG)
        {
          syslog(LOG_NOTICE, "COMPARER: Comparing file: %s", path);
          if (strcmp(path, destinationPath))
          {
            syslog(LOG_NOTICE, "COMPARER: Deleting file: %s", path);
            remove(path);
          }
        }
      }
    }
  }
  closedir(dir);
}

off_t sizeGetter(char *size)
{
    struct stat _size;
    if(stat(size,&_size) == 0) return _size.st_size;
    return -99;
}





































































