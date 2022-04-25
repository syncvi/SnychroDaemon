#ifndef FUNNIES_H
#define FUNNIES_H
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
/*--------------FUNCTIONS GO THERE--------------*/
void instruction_manual();
time_t timeGetter(char* time);
off_t sizeGetter(char* size);
int appendSlash(char* entry_path, int path_len);
void deleteAll(char* givenPath);
void copyFiles(char* source, char* destination);
void browseDirectories(char* sourcePath, char* destinationPath, int isRecursive);
void compareFiles(char* sourcePath, char* destinationPath);
#endif
