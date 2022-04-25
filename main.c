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
int siema=3;
void handler(int signum){
    if (signum == SIGUSR1)
    {
        syslog (LOG_NOTICE, "SIGUSR1 provided...");
        syslog (LOG_NOTICE, "Waking up the d(a)emon...");
        flag = 1;
    }
}

void bigSleeping(int time){
    int i = 0;
    while(i<time)
    {
        if(flag == 1) return;

        sleep(1);
        i++;
    }
}

int main(int argc, char* argv[]){

    
    openlog("PROJECT: DEMON", LOG_PID|LOG_CONS, LOG_USER);

    if(signal(SIGUSR1, handler) == SIG_ERR) syslog(LOG_ERR, "MAIN: Signal was not received correctly");

    if(argc<3)
    {   
        if(strncmp(argv[1], "h", 16) || strncmp(argv[1], "help", 16)){
            instruction_manual();
            return 0;
        } 
        else{
            syslog(LOG_ERR,"MAIN: Too few arguments!");
            fprintf(stdout, "Too few arguments! Example:\n");
            fprintf(stdout, "./program /source /destination --additional-flags\n");
            exit(EXIT_FAILURE);
        }
        
        
    } //a change

    syslog(LOG_NOTICE, "MAIN: Task has been started.");

    int choice;
    //if flag requires additional argument, like -s [filesize], thus, another switch will use choiceOption
    int choiceOption; 
    int isRecursive = 0;

    char* source = argv[1];
    char* destination = argv[2];
    struct stat status1;
    struct stat status2; //for testing a file type
    //https://www.gnu.org/software/libc/manual/html_node/Testing-File-Type.html

    if(stat(source, &status1) == 0)
        if(!S_ISDIR(status1.st_mode)){
            syslog(LOG_ERR, "MAIN: First argument is not a folder!");
            fprintf(stderr,"First argument is not a folder!\n");
            exit(EXIT_FAILURE);
        }
    
    if(stat(destination, &status2) == 0)
        if(!S_ISDIR(status2.st_mode)){
            syslog(LOG_ERR, "MAIN: Second argument is not a folder!");
            fprintf(stderr,"Second argument is not a folder!\n");
            exit(EXIT_FAILURE);
        }
    
    if(strcmp(source,destination) == 0){
          fprintf(stderr,"Source and destination directory are the same.\n");
          exit(EXIT_FAILURE);
        }

    while((choice=getopt(argc,argv,"Rd:")) != -1){ //ADD MORE CASES LATER -t, -c, -s, w/e
        switch (choice){
            case 'R':
            isRecursive=1;
            break;


            default:
            abort(); //or simply break, idk
        }
    }












/*------------------------------------------------CREATING A DAEMON------------------------------------*/
    pid_t pid, sid;
        
        pid = fork();

        if (pid < 0) {
            syslog(LOG_ERR,"MAIN: Fork function encountered an error.");
            exit(EXIT_FAILURE);
        }
        if (pid > 0)  exit(EXIT_SUCCESS);
                
        
    umask(0);
                 
    sid = setsid();

        if (sid < 0) {
                syslog(LOG_ERR,"MAIN: Setting sid has failed.");
                exit(EXIT_FAILURE);
        }
        
        if ((chdir("/")) < 0) {
                syslog(LOG_ERR,"MAIN: chdir has failed.");
                exit(EXIT_FAILURE);
        }
/*----------------------------------------------------------------------------------------------------*/
    return 0;
}