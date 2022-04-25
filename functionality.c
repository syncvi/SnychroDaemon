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
time_t timeGetter(char* time)
{
  struct stat _time;
  if (stat(time, &_time) == -1)
  {
    syslog(LOG_ERR, "Sth gone wrong with setting time!!! %s", time);
  }
  return _time.st_mtime;
}
off_t sizeGetter(char* size)
{
  struct stat _size;
  if (stat(size, &_size) == 0)
    return _size.st_size;
  return -99;
}
void instruction_manual()
{
  syslog(LOG_NOTICE, "Displaying manual page...");
  char *filename = "manual.txt";
  FILE *fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("Error: could not open file %s", filename);
    return;
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
void deleteAll(char* givenPath) // wiper
{
  DIR *dir;
  struct dirent *entry;
  char path[1024];
  if (!(dir = opendir(givenPath)))
  {
    syslog(LOG_ERR, "WIPER: Could not open directory");
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


void copyFiles(char* source, char* destination)
{
  DIR* dir;
  struct dirent* entry;
  char path[1024];
  appendSlash(source, strlen(source));
  appendSlash(destination, strlen(destination));
  if (!(dir = opendir(source)))
  {
    syslog(LOG_ERR, "COPY: Could not open directory");
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(path, 1024, "%s/%s", source, entry->d_name);
      if (entry->d_type == DT_DIR)
      {
        copyFiles(path, destination);
      }
      else
      {
        int fd_src = open(path, O_RDONLY);
        int fd_dst = open(destination, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
        if (fd_src == -1 || fd_dst == -1)
        {
          syslog(LOG_ERR, "COPY: Could not open file");
          return;
        }
        if (sendfile(fd_dst, fd_src, NULL, sizeGetter(path)) == -1)
        {
          syslog(LOG_ERR, "COPY: Could not copy file");
          return;
        }
      }
    }
  }
  closedir(dir);
}
void browseDirectories(char* sourcePath, char* destinationPath, int isRecursive)
{
  DIR* dir;
  struct dirent* entry;
  char path[1024];
  appendSlash(sourcePath, strlen(sourcePath));
  appendSlash(destinationPath, strlen(destinationPath));
  if (!(dir = opendir(sourcePath)))
  {
    syslog(LOG_ERR, "BROWSE: Could not open directory");
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(path, 1024, "%s/%s", sourcePath, entry->d_name);
      if (entry->d_type == DT_DIR)
      {
        if (isRecursive)
        {
          browseDirectories(path, destinationPath, isRecursive);
        }
      }
      else
      {
        copyFiles(path, destinationPath);
      }
    }
  }
  closedir(dir);
}
