#!/bin/sh
#make clean
#make
#./syndi /home/ogar/Desktop/test /home/ogar/Desktop/test13 -R -T 0 -S 1024
#pidof ./syndi - gets the process id

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
make clean 
make 

echo "Test 1 - Simplest copying of files from FolderA to FolderB"
echo "Copying will be nonrecursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function"

rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -T 0 -S 32000000
sleep 1s
echo "Copying done listing of FolderA"
ls $SCRIPTPATH/FolderA
echo "Copying done listing of FolderB"
ls $SCRIPTPATH/FolderB   
echo "Test 1 - Finished"
echo " "

echo "Test 2 - Copying of files from FolderA to FolderB with recursive option"
echo "Copying will be recursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function"
rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -T 0 -S 32000000
sleep 1s
echo "Copying done listing of FolderA"
ls FolderA
echo "Copying done listing of FolderA/SubFolderA"
ls FolderA/SubFolderA
echo "Copying done listing of FolderB"
ls FolderB
echo "Copying done listing of FolderB/SubFolderA"
ls FolderB/SubFolderA
echo "Test 2 - Finished"
echo " "

echo "Test 3 - Copying of files from FolderA to FolderB with files in FolderB that do not exist in FolderA"
echo "Copying will be nonrecursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function"
rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB
mkdir FolderB/SubFolderB
dd if=/dev/zero of=FolderB/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/plik8MB.txt count=8 bs=1MB status=none
echo "Listing of FolderA before copying"
ls FolderA
echo "Listing of FolderB before copying"
ls FolderB
echo " "

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -T 0 -S 32000000
sleep 1s
echo "Copying done listing of FolderA"
ls FolderA
echo "Copying done listing of FolderA/SubFolderA"
ls FolderA/SubFolderA
echo "Copying done listing of FolderB"
ls FolderB
echo "Test 3 - Finished"
echo " "

echo "Test 4 - Copying files from FolderA to FolderB with folder FolderB containing SubFolderC with files, that do not exist in FolderA"
echo "Copying will be recursive, be concluded with default time and use 32MB as maximum file size for small function"
echo "This test will take a long time to finish, so SIGUSR1 will be sent to the process after 5 seconds"
rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB
mkdir FolderB/SubFolderB
mkdir FolderB/SubFolderB/SubFolderC
dd if=/dev/zero of=FolderB/SubFolderB/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/SubFolderC/plik8MB.txt count=8 bs=1MB status=none

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -S 32000000
sleep 5s
pidprogramu="$(pidof ./syndi)"
kill -s SIGUSR1 $pidprogramu
sleep 1s
echo "Copying done listing of FolderA"
ls FolderA
echo "Copying done listing of FolderA/SubFolderA"
ls FolderA/SubFolderA
echo "Copying done listing of FolderB"
ls FolderB
echo "Copying done listing of FolderB/SubFolderA"
ls FolderB/SubFolderA
echo "Test 4 - Finished"
echo " "


echo " "
echo "Opening syslog"
#Write only demon logs to syslog
cat /var/log/syslog | grep "DEMON">> $SCRIPTPATH/syslog.txt
#display syslog.txt
#cat $SCRIPTPATH/syslog.txt
echo "Closing syslog"
echo " "


