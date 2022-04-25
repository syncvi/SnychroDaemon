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
    char *filename = "manual.txt";
    FILE *fp = fopen(filename, "r");

    if (fp == NULL)
    {
        printf("Error: could not open file %s", filename);
        return 1;
    }

    // read one character at a time and
    // display it to the output
    char ch;
    while ((ch = fgetc(fp)) != EOF)
        putchar(ch);

    // close the file
    fclose(fp);
}
void deleteAll(char* givenPath)
{
    syslog(LOG_NOTICE, "Erasing  everything from: %s...\n ", givenPath);

}