# SynchroDaemon
--a very cool project with a typo in the url cuz i'm a dumbass


## Valgrind Dependencies (testing only)
### Ubuntu, Debian, etc.
sudo apt-get install valgrind -y  
### RHEL, CentOS, Fedora, etc.
sudo yum install valgrind -y
### Arch, Manjaro, Garuda, etc
sudo pacman -Syu valgrind

# Usage/Manual
1. Parameters: source path, destination path
2. Options :

      -R - browsing directories recursively, also it doesn't ignore directories inside directories anymore.
      
      -t - set time, daemon will start synchronizing directories after 5 minutes (300s) by default.
      
      -s - set file size, depending on given size, different functionality will be triggered.
      
      -h - displaying manual.
      
# Useful bash commands:

ps aux - displaying all processes

kill -s SIGUSR1 [pid] where pid is an id of your daemon process (remove square brackets).
perl -MDate::Parse -ne 'print if/^(.{15})\s/&&str2time($1)>time-60'
