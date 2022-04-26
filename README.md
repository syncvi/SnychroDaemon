# SynchroDaemon
--a very cool project with a typo in the url cuz i'm a dumbass

# Install Dependencies:
...

# Usage/Manual
1. Parameters: source path, destination path
2. Options :
      -R - browsing directories recursively, also it doesn't ignore directories inside directories anymore.
      
      -t - set time, daemon will start synchronizing directories after 5 minutes (300s) by default.
      
      -s - set file size, depending on given size, different functionality will be triggered.
      
      -h - displaying manual.
      
# Useful bash commands:
ps aux - displaying all processes
kill -s SIGUSR1 [pid] where pid is an id of you daemon process (remove square brackets).
