#include "funnies.h"
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
void instruction_manual()
{
    syslog(LOG_NOTICE, "Displaying manual page...");
    fprintf(stdout, "This is where functionality will be explained...one day. (lmao) (rofl)");
}