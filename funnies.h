#ifndef FUNNIES_H
#define FUNNIES_H
#include "funnies.h"
<<<<<<< HEAD
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
void deleteAll(char* givenPath);
time_t timeGetter(char *time);
=======

/*--------------FUNCTIONS GO THERE--------------*/
void instruction_manual();
void deleteAll(char* givenPath);
time_h timeGetter(char *time);
off_t sizeGetter(char *size);
>>>>>>> 4eef5e08709c0531c41098c745c380c82e6c6b7d
#endif
