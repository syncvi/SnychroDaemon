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