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

// create a function that copies files from source to destination if files are different, if files are the same, do nothing, ignore directories inside, can be recursive
void copyFiles(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "Kopiowanie dużego pliku:%s do %s poprzez funkcję sendfile", entry_path_source, entry_path_destination);
  int source_fd;
  int destination_fd;
  struct stat stat_buf;
  off_t offset = 0;
  printf("%s\n", entry_path_source);
  printf("%s\n", entry_path_destination);
  source_fd = open(entry_path_source, O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH);
  if (source_fd == -1)
  {
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }
  fstat(source_fd, &stat_buf);
  destination_fd = open(entry_path_destination, O_WRONLY | O_CREAT, stat_buf.st_mode);
  if (destination_fd == -1)
  {
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  if (sendfile(destination_fd, source_fd, &offset, stat_buf.st_size) == -1)
  {
    perror("Sendfile error:");
    exit(EXIT_FAILURE);
  }

  close(source_fd);
  close(destination_fd);
}
void browseDirectories(char* sourcePath, char* destinationPath, int isRecursive)
{
  DIR* dir;
  struct dirent* entry;
  char path[1024];
  char destination[1024];
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
      snprintf(destination, 1024, "%s/%s", destinationPath, entry->d_name);
      if (entry->d_type == DT_DIR)
      {
        if (isRecursive)
        {
          browseDirectories(path, destinationPath, isRecursive);
        }
      }
      else
      {
        copyFiles(path, destination);
      }
    }
  }
  closedir(dir);
}
