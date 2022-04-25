#include "funnies.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <signal.h>
#include <getopt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
void instruction_manual()
{
    syslog(LOG_NOTICE, "Displaying manual page...");
    fprintf(stdout, "This is where functionality will be explained...one day. (lmao) (rofl)");
}
void deleteAll(char* givenPath)
{
    syslog(LOG_NOTICE, "Erasing  everything from: %s...\n ", givenPath);

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