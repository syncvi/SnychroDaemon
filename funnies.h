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
void browseDirectories(char* sourcePath, char* destinationPath, int isRecursive, int size);
#endif
