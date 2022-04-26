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
volatile int flag = 0;
//TODO: formating and shit
void handler(int signum)
{
    if (signum == SIGUSR1)
    {
        syslog(LOG_NOTICE, "MAIN: SIGUSR1 provided...");
        syslog(LOG_NOTICE, "MAIN: Waking up the d(a)emon...");
        flag = 1;
    }
}
void bigSleeping(int time)
{
    int i = 0;
    while (i < time)
    {
        if (flag == 1) return;
            
        sleep(1);
        i++;
    }
}
int main(int argc, char* argv[])
{
    /*------------------------------------Required inputs check------------------------------------*/
    openlog("PROJECT: DEMON", LOG_PID | LOG_CONS, LOG_USER);

    if (signal(SIGUSR1, handler) == SIG_ERR)
        syslog(LOG_ERR, "MAIN: Signal was not received correctly");

    if (argc < 3)
    {
        syslog(LOG_ERR, "MAIN: Not enough arguments provided");
        printf("Not enough arguments provided\n");
        printf("Would you like to see the manual page? (y/n)\n");
        char answer;
        scanf("%c", &answer); //TODO: maybe use a safer function like sscanf instead of scanf
        if (answer == 'y' || answer == 'Y')
        {
            instruction_manual();
            syslog(LOG_NOTICE, "MAIN: Manual page displayed");
            exit(1);
        }
        else
        {
            syslog(LOG_NOTICE, "MAIN: No manual page displayed");
            exit(1);
        }
    }   
    /*------------------------------------Optional inputs check------------------------------------*/
    syslog(LOG_NOTICE, "MAIN: Task has been started.");

    int choice;
    int isRecursive = 0;
    int time = 300;
    int filesize = __INT_MAX__;

    char* source = argv[1];
    char* destination = argv[2];
    struct stat status1;
    struct stat status2; // for testing a file type
    while ((choice = getopt(argc, argv, "Rh:T:S:")) != -1)
    {
        switch (choice)
        {
        case 'R':
            isRecursive = 1;
            break;
        case 'T':
            time = atoi(optarg);
            break;
        case 'S':
            filesize = atoi(optarg);
            break;
        case '?':
            switch (optopt)
            {
            case 'T':
                syslog(LOG_ERR, "MAIN: Time option is invalid!");
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                exit(EXIT_FAILURE);
                break;
            case 'S':
                syslog(LOG_ERR, "MAIN: Filesize option is invalid!");
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                exit(EXIT_FAILURE);
                break;
            }
            syslog(LOG_ERR, "MAIN: Unknown argument was given!");
            fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        default:
            abort();
        }
    }
    /*------------------------------------Creating a Daemon------------------------------------*/
    pid_t pid, sid;

    pid = fork();

    if (pid < 0)
    {
        syslog(LOG_ERR, "MAIN: Fork function encountered an error.");
        exit(EXIT_FAILURE);
    }
    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    sid = setsid();

    if (sid < 0)
    {
        syslog(LOG_ERR, "MAIN: Setting sid has failed.");
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "MAIN: chdir has failed.");
        exit(EXIT_FAILURE);
    }
    /*------------------------------------Directories check------------------------------------*/
    bigSleeping(time);
    if (stat(source, &status1) == 0 && !S_ISDIR(status1.st_mode))
    {
        syslog(LOG_ERR, "MAIN: First argument is not a folder or doesn't exist!");
        fprintf(stderr, "First argument is not a folder or doesn't exist!\n");
        exit(EXIT_FAILURE);
    }

    if (stat(destination, &status2) == 0 && !S_ISDIR(status2.st_mode))
    {
        syslog(LOG_ERR, "MAIN: Second argument is not a folder or doesn't exist!!");
        fprintf(stderr, "Second argument is not a folder or doesn't exist!!\n");
        exit(EXIT_FAILURE);
    }

    if (strcmp(source, destination) == 0)
    {
        syslog(LOG_ERR, "MAIN: Source and destination are the same!");
        fprintf(stderr, "Source and destination directory are the same!\n");
        exit(EXIT_FAILURE);
    }
    /*------------------------------------Copying Files------------------------------------*/
    browseDirectories(source, destination, isRecursive, filesize);
    syslog(LOG_NOTICE, "MAIN: Wrapping things up...");
    sleep(1);
    syslog(LOG_NOTICE, "MAIN: Task has been finished.");
    closelog();
    exit(EXIT_SUCCESS);
}
