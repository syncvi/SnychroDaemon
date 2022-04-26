#define _GNU_SOURCE
#include "funnies.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dirent.h>
#include <time.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <utime.h>


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

// Check if modification date of two files are equal
bool isModified(char *path1, char *path2)
{
  struct stat _time1;
  struct stat _time2;
  if (stat(path1, &_time1) == -1)
  {
    syslog(LOG_ERR, "IS_MODIFIED: Error while checking time. %s", path1);
  }
  if (stat(path2, &_time2) == -1)
  {
    syslog(LOG_ERR, "IS_MODIFIED: Error while checking time. %s", path2);
  }
  if (_time1.st_mtime == _time2.st_mtime)
  {
    return 1;
  }
  return 0;
}

// set modification date of destination file to source file
void setModified(char *source, char *destination)
{
  struct stat source_time;
  if (stat(source, &source_time) == -1)
  {
    syslog(LOG_ERR, "SETMODIFIED: Error while checking time. %s", source);
  }
  struct utimbuf destination_time;
  destination_time.modtime = source_time.st_mtime;
  if (utime(destination, &destination_time) == -1)
  {
    syslog(LOG_ERR, "SETMODIFIED: Error while setting time. %s", destination);
  }
}

// check if file already exists in destination
int checkIfFileExists(char *sourcePath, char *destinationPath)
{
  struct stat s_copy, d_copy;
  if (stat((sourcePath), &s_copy) == -1)
  {
    syslog(LOG_ERR, "CHECK: Error while checking if source file exists: %s", sourcePath);
    printf("Error: could not open file %s", sourcePath);
  }
  stat((destinationPath), &d_copy);
  if (s_copy.st_size == d_copy.st_size)
  {
    if (isModified(sourcePath, destinationPath))
    {
      return 1;
    }
    else
    {
      setModified(sourcePath, destinationPath);
      return 1;
    }
  }
  else
  {
    return 0;
  }
}

// check if destination directory exists
int checkIfDirectoryExists(char *path)
{
  struct stat s_copy;
  stat(path, &s_copy);
  if (S_ISDIR(s_copy.st_mode))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

//remove directory and all directories and files within it
void removeDirectory(char *path)
{
  DIR *dir;
  struct dirent *entry;
  char pathname[PATH_MAX];
  if (!(dir = opendir(path)))
  {
    syslog(LOG_ERR, "REMOVE_DIR: Error while opening directory: %s", path);
    printf("Error: could not open directory %s", path);
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(pathname, (size_t)PATH_MAX, "%s/%s", path, entry->d_name);
      if (entry->d_type == DT_DIR)
        removeDirectory(pathname);
      if (entry->d_type == DT_REG)
      {
        if (remove(pathname) == -1)
        {
          syslog(LOG_ERR, "REMOVE_DIR: Error while removing file: %s", pathname);
          printf("Error: could not remove file %s", pathname);
        }
      }

    }
  }
  closedir(dir);
  rmdir(path);
}
// Function that copies files from source to destination using sendfile
/*void copyBigFiles(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "COPY: Copying big files from %s to %s", entry_path_source, entry_path_destination);
  int source_fd;
  int destination_fd;
  struct stat stat_buf;
  off_t offset = 0;
  source_fd = open(entry_path_source, O_RDONLY, S_IRWXU | S_IRWXG | S_IROTH);
  if (source_fd == -1)
  {
    syslog(LOG_ERR, "COPY: Error opening source file %s while copying big files", entry_path_source);
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }
  fstat(source_fd, &stat_buf);
  destination_fd = open(entry_path_destination, O_WRONLY | O_CREAT, stat_buf.st_mode);
  if (destination_fd == -1)
  {
    syslog(LOG_ERR, "COPY: Error opening destination file %s while copying big files", entry_path_destination);
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  if (sendfile(destination_fd, source_fd, &offset, stat_buf.st_size) == -1)
  {
    syslog(LOG_ERR, "COPY: Error copying big files from %s to %s", entry_path_source, entry_path_destination);
    perror("Sendfile error:");
    exit(EXIT_FAILURE);
  }

  close(source_fd);
  close(destination_fd);
}*/

// Copying big files from source to destination using mapping
void copyBigFiles(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "COPY_BIG: Copying big files from %s to %s", entry_path_source, entry_path_destination);
  int source_fd;
  int destination_fd;
  struct stat stat_buf;
  off_t offset = 0;
  source_fd = open(entry_path_source, O_RDONLY);
  if (source_fd == -1)
  {
    syslog(LOG_ERR, "COPY_BIG: Error opening source file %s while copying big files", entry_path_source);
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }
  if (fstat(source_fd, &stat_buf))
  {
    syslog(LOG_ERR, "COPY_BIG: Error getting file size of %s while copying big files", entry_path_source);
    perror("Fstat error:");
    exit(EXIT_FAILURE);
  }
  destination_fd = open(entry_path_destination, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (ftruncate(destination_fd, stat_buf.st_size))
  {
    syslog(LOG_ERR, "COPY_BIG: Error truncating file %s while copying big files", entry_path_destination);
    perror("Ftruncate error:");
    exit(EXIT_FAILURE);
  }
  if (destination_fd == -1)
  {
    syslog(LOG_ERR, "COPY_BIG: Error opening destination file %s while copying big files", entry_path_destination);
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  char *source_map = mmap(NULL, stat_buf.st_size, PROT_READ, MAP_PRIVATE, source_fd, 0);
  if (source_map == MAP_FAILED)
  {
    syslog(LOG_ERR, "COPY_BIG: Error mapping source file %s while copying big files", entry_path_source);
    perror("Source_map error:");
    exit(EXIT_FAILURE);
  }
  char *destination_map = mmap(NULL, stat_buf.st_size, PROT_WRITE, MAP_SHARED, destination_fd, 0);
  if (destination_map == MAP_FAILED)
  {
    syslog(LOG_ERR, "COPY_BIG: Error mapping destination file %s while copying big files", entry_path_destination);
    perror("Destination_map error:");
    exit(EXIT_FAILURE);
  }
  memcpy(destination_map, source_map, stat_buf.st_size);
  munmap(source_map, stat_buf.st_size);
  munmap(destination_map, stat_buf.st_size);
  close(source_fd);
  close(destination_fd);
}

// Function copying files smaller than gives size if parameter -T was used
void copySmallFiles(char *entry_path_source, char *entry_path_destination)
{
  syslog(LOG_NOTICE, "COPY_SMALL: Copying small files from %s to %s", entry_path_source, entry_path_destination);
  int src_fd, dst_fd, n;
  unsigned char buffer[4096];

  src_fd = open(entry_path_source, O_RDONLY);
  if (src_fd == -1)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error opening source file %s while copying small files", entry_path_source);
    printf("Error opening source file: %s\n", entry_path_source);
    exit(EXIT_FAILURE);
  }

  dst_fd = open(entry_path_destination, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IROTH);
  if (dst_fd == -1)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error opening destination file %s while copying small files", entry_path_destination);
    printf("Error opening destination file: %s\n", entry_path_destination);
    exit(EXIT_FAILURE);
  }

  while ((n = read(src_fd, buffer, sizeof(buffer))) > 0)
  {
    if (write(dst_fd, buffer, n) != n)
    {
      syslog(LOG_ERR, "COPY_SMALL: Error writing to destination file %s while copying small files", entry_path_destination);
      printf("Error writing to destination file: %s\n", entry_path_destination);
      exit(EXIT_FAILURE);
    }
  }
  if (n < 0)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error reading from source file %s while copying small files", entry_path_source);
    printf("Error reading from source file: %s\n", entry_path_source);
    exit(EXIT_FAILURE);
  }
  close(src_fd);
  close(dst_fd);
}

// Function checking which copy function to use
void howToCopy(char *sourcePath, char *destinationPath, int size)
{
  struct stat f_copy;
  if (stat((sourcePath), &f_copy) == -1)
  {
    syslog(LOG_ERR, "HT_COPY: Error getting file stats while copying %s to %s", sourcePath, destinationPath);
    perror("Error getting file stats");
    exit(EXIT_FAILURE);
  }
  if ((f_copy.st_size) >= size)
    copyBigFiles(sourcePath, destinationPath);
  else
    copySmallFiles(sourcePath, destinationPath);
}
// Browse source and destination directory. If there is file in source which does not exist in destination, remove it.
void browseSourceAndDestination(char *sourcePath, char *destinationPath, int isRecursive)
{
  DIR *dir;
  struct dirent *entry;
  char path[1024];
  char destination[1024];
  if (!(dir = opendir(sourcePath)))
  {
    syslog(LOG_ERR, "BROWSER_SAD: Could not open directory");
    perror("Error opening directory");
    return;
  }
  while ((entry = readdir(dir)) != NULL)
  {
    if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
    {
      snprintf(path, 1024, "%s/%s", sourcePath, entry->d_name);
      snprintf(destination, 1024, "%s/%s", destinationPath, entry->d_name);
      if (entry->d_type == DT_REG)
      {
        if (checkIfFileExists(path, destination) == 0)
        {
          remove(path);
          syslog(LOG_NOTICE, "BROWSER_SAD: File %s was removed since %s file doesn't exist", path, destination);
        }
      }
      if (entry->d_type == DT_DIR)
      {
        if (checkIfDirectoryExists(destination) == 0 && checkIfDirectoryExists(path) == 1 && isRecursive == 1)
        {
          removeDirectory(path);
          syslog(LOG_NOTICE, "BROWSER_SAD: Directory %s was removed since %s directory doesn't exist ", path,destination);
        }
      }
    }
  }
}

// Main function browsing given directory and copying files to destination
void browseDirectories(char *sourcePath, char *destinationPath, int isRecursive, int size)
{
  DIR *dir;
  struct dirent *entry;
  char path[1024];
  char destination[1024];
  if (!(dir = opendir(sourcePath)))
  {
    syslog(LOG_ERR, "BROWSER_DIR: Could not open directory");
    return;
  }

  snprintf(path, 1024, "%s", sourcePath);
  snprintf(destination, 1024, "%s", destinationPath);
  browseSourceAndDestination(destination, path, isRecursive);

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
          if (checkIfDirectoryExists(destination) == 0)
          {
            syslog(LOG_NOTICE, "BROWSER_DIR: Creating directory %s", destination);
            mkdir(destination, S_IRWXU | S_IRWXG | S_IROTH);
          }
          browseDirectories(path, destination, isRecursive, size);
        }
      }

      else
      {
        if (checkIfFileExists(path, destination) == 1)
        {
          syslog(LOG_NOTICE, "BROWSER_DIR: File %s already exists in %s", entry->d_name, destinationPath);
        }
        else
        {
          howToCopy(path, destination, size);
        }
      }
    }
  }
  closedir(dir);
}
