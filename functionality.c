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
  char* filename = "manual.txt";
  FILE* fp = fopen(filename, "r");

  if (fp == NULL)
  {
    printf("MAN_ERR: could not open file %s", filename);
    return;
  }
  char ch;
  while ((ch = fgetc(fp)) != EOF)
    putchar(ch);
  fclose(fp);
}

// Check if modification date of two files are equal
bool isModified(char* path1, char* path2)
{
  struct stat _time1;
  struct stat _time2;
  if (stat(path1, &_time1) == -1)
  {
    syslog(LOG_ERR, "IS_MODIFIED: Error while checking time in: %s", path1);
  }
  if (stat(path2, &_time2) == -1)
  {
    syslog(LOG_ERR, "IS_MODIFIED: Error while checking time in: %s", path2);
  }
  if (_time1.st_mtime == _time2.st_mtime)
  {
    return 1;
  }
  return 0;
}
// set modification date of destination file to source file
void setModified(char* source, char* destination)
{
  struct stat source_time;
  if (stat(source, &source_time) == -1)
  {
    syslog(LOG_ERR, "SET_MODIFIED: Error while checking time in: %s", source);
  }
  struct utimbuf destination_time;
  destination_time.modtime = source_time.st_mtime;
  if (utime(destination, &destination_time) == -1)
  {
    syslog(LOG_ERR, "SET_MODIFIED: Error while setting time in: %s", destination);
  }
}

// check if file already exists in destination
int checkIfFileExists(char* sourcePath, char* destinationPath)
{
  struct stat s_copy, d_copy;
  if (stat((sourcePath), &s_copy) == -1)
  {
    syslog(LOG_ERR, "CHECK_FILE: Error while checking if source file exists: %s", sourcePath);
    printf("Error: could not open file: %s", sourcePath);
  }
  stat((destinationPath), &d_copy);
  if (s_copy.st_ino == d_copy.st_ino)
  {
    if (s_copy.st_size == d_copy.st_size)
    {
     if (isModified(sourcePath, destinationPath)) return 1;
     else
      {
        setModified(sourcePath, destinationPath);
        return 1;
      }
    }
    else return 0; 
  }
  else return 0; 
}

// check if destination directory exists
int checkIfDirectoryExists(char* path)
{
  struct stat s_copy;
  stat(path, &s_copy);

  if (S_ISDIR(s_copy.st_mode)) return 1;
  else return 0;
}

//remove directory and all directories and files within it
void removeDirectory(char* path)
{
  DIR* dir;
  struct dirent* entry;
  char pathname[PATH_MAX];
  if (!(dir = opendir(path)))
  {
    syslog(LOG_ERR, "REMOVE_DIR: Error while opening directory: %s", path);
    printf("Error: could not open directory: %s", path);
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
          printf("Error: could not remove file: %s", pathname);
        }
      }

    }
  }
  closedir(dir);
  rmdir(path);
}

// Copying big files from source to destination using mapping
void copyBigFiles(char* sourcePath, char* destinationPath)
{
  syslog(LOG_NOTICE, "COPY_BIG: Copying files from: %s to: %s", sourcePath, destinationPath);
  int sourceFolder;
  int destinationFolder;
  struct stat statusBuffer;
  sourceFolder = open(sourcePath, O_RDONLY);
  if (sourceFolder == -1)
  {
    syslog(LOG_ERR, "COPY_BIG: Error opening source file %s while copying big files", sourcePath);
    perror("Source_fd error:");
    exit(EXIT_FAILURE);
  }
  if (fstat(sourceFolder, &statusBuffer))
  {
    syslog(LOG_ERR, "COPY_BIG: Error getting file size of %s while copying big files", sourcePath);
    perror("Fstat error:");
    exit(EXIT_FAILURE);
  }
  destinationFolder = open(destinationPath, O_RDWR | O_CREAT | O_TRUNC, 0644);
  if (ftruncate(destinationFolder, statusBuffer.st_size))
  {
    syslog(LOG_ERR, "COPY_BIG: Error truncating file %s while copying big files", destinationPath);
    perror("Ftruncate error:");
    exit(EXIT_FAILURE);
  }
  if (destinationFolder == -1)
  {
    syslog(LOG_ERR, "COPY_BIG: Error opening destination file %s while copying big files", destinationPath);
    perror("Destination_fd error:");
    exit(EXIT_FAILURE);
  }
  char* source_map = mmap(NULL, statusBuffer.st_size, PROT_READ, MAP_PRIVATE, sourceFolder, 0);
  if (source_map == MAP_FAILED)
  {
    syslog(LOG_ERR, "COPY_BIG: Error mapping source file %s while copying big files", sourcePath);
    perror("Source_map error:");
    exit(EXIT_FAILURE);
  }
  char* destination_map = mmap(NULL, statusBuffer.st_size, PROT_WRITE, MAP_SHARED, destinationFolder, 0);
  if (destination_map == MAP_FAILED)
  {
    syslog(LOG_ERR, "COPY_BIG: Error mapping destination file %s while copying big files", destinationPath);
    perror("Destination_map error:");
    exit(EXIT_FAILURE);
  }
  memcpy(destination_map, source_map, statusBuffer.st_size);
  munmap(source_map, statusBuffer.st_size);
  munmap(destination_map, statusBuffer.st_size);
  close(sourceFolder);
  close(destinationFolder);
}

// Function copying files smaller than gives size if parameter -T was used
void copySmallFiles(char* sourcePath, char* destinationPath)
{
  syslog(LOG_NOTICE, "COPY_SMALL: Copying files from %s to %s", sourcePath, destinationPath);
  int sourceFolder, destinationFolder, reader;
  unsigned char buffer[4096];

  sourceFolder = open(sourcePath, O_RDONLY);
  if (sourceFolder == -1)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error opening source file %s while copying small files", sourcePath);
    printf("Error opening source file: %s\n", sourcePath);
    exit(EXIT_FAILURE);
  }

  destinationFolder = open(destinationPath, O_WRONLY | O_CREAT | O_EXCL, S_IRWXU | S_IRWXG | S_IROTH);
  if (destinationFolder == -1)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error opening destination file %s while copying small files", destinationPath);
    printf("Error opening destination file: %s\n", destinationPath);
    exit(EXIT_FAILURE);
  }

  while ((reader = read(sourceFolder, buffer, sizeof(buffer))) > 0)
  {
    if (write(destinationFolder, buffer, reader) != reader)
    {
      syslog(LOG_ERR, "COPY_SMALL: Error writing to destination file %s while copying small files", destinationPath);
      printf("Error writing to destination file: %s\n", destinationPath);
      exit(EXIT_FAILURE);
    }
  }
  if (reader < 0)
  {
    syslog(LOG_ERR, "COPY_SMALL: Error reading from source file %s while copying small files", sourcePath);
    printf("Error reading from source file: %s\n", sourcePath);
    exit(EXIT_FAILURE);
  }
  close(sourceFolder);
  close(destinationFolder);
}

// Function checking which copy function to use
void howToCopy(char* sourcePath, char* destinationPath, int size)
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
void browseSourceAndDestination(char* sourcePath, char* destinationPath, int isRecursive)
{
  DIR* dir;
  struct dirent* entry;
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
  closedir(dir);
}

// Main function browsing given directory and copying files to destination
void browseDirectories(char* sourcePath, char* destinationPath, int isRecursive, int size)
{
  DIR* dir;
  struct dirent* entry;
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