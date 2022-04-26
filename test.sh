#!/bin/sh

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
make clean 
make 
rm -rf syslog.txt
rm -rf valgrindlogs.txt
#sudo truncate -s 0 /var/log/syslog
echo "Test 1 - Simplest copying of files from FolderA to FolderB."
echo "Copying will be nonrecursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function."

rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -t 0 -s 32000000
sleep 1s
echo "Copying done; listing FolderA..."
ls $SCRIPTPATH/FolderA
echo "Copying done; listing FolderB..."
ls $SCRIPTPATH/FolderB   
echo "Test 1 - Finished"
echo " "

echo "Test 2 - Copying files from FolderA to FolderB with recursive option."
echo "Copying will be recursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function."
rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -t 0 -s 32000000
sleep 1s
echo "Copying done; listing FolderA..."
ls FolderA
echo "Copying done; listing FolderA/SubFolderA..."
ls FolderA/SubFolderA
echo "Copying done; listing FolderB..."
ls FolderB
echo "Copying done; listing FolderB/SubFolderA..."
ls FolderB/SubFolderA
echo "Test 2 - Finished"
echo " "

echo "Test 3 - Copying files from FolderA to FolderB with files in FolderB that do not exist in FolderA."
echo "Copying will be nonrecursive, be concluded instantly (for ease of testing) and use 32MB as maximum file size for small function."
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
echo "Listing FolderA before copying..."
ls FolderA
echo "Listing FolderB before copying..."
ls FolderB
echo " "

sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -t 0 -s 32000000
sleep 1s
echo "Copying done; listing FolderA..."
ls FolderA
echo "Copying done; listing FolderA/SubFolderA..."
ls FolderA/SubFolderA
echo "Copying done; listing FolderB..."
ls FolderB
echo "Test 3 - Finished"
echo " "

echo "Test 4 - Copying files from FolderA to FolderB with folder FolderB containing SubFolderC with files, that do not exist in FolderA."
echo "Copying will be recursive, be concluded with default time and use 32MB as maximum file size for small function".
echo "This test will take a long time to finish, so SIGUSR1 will be sent to the process after 5 seconds..."
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
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -s 32000000
sleep 5s
pidprogramu="$(pidof ./syndi)"
kill -s SIGUSR1 $pidprogramu
sleep 1s
echo "Copying done; listing FolderA..."
ls FolderA
echo "Copying done; listing FolderA/SubFolderA..."
ls FolderA/SubFolderA
echo "Copying done; listing FolderB..."
ls FolderB
echo "Copying done; listing FolderB/SubFolderA..."
ls FolderB/SubFolderA
echo "Test 4 - Finished"
echo " "
echo " "

echo "Test 5 - Valgrind"
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
valgrind --leak-check=yes --track-origins=yes --log-file=valgrindlogs.txt ./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -t 0 -s 32000000
sleep 1s
echo "Concluding Test 5..."
sleep 1s
echo "It doesn't take any longer than 0.0001s, but it looks cool if there's a pause inbetween, lol."
sleep 1s
echo "Test 5 - Finished"
echo " "


echo " "
echo "Opening syslog..."
sleep 1s
echo "Writing to local file: syslog.txt..."
sleep 1s
sed -n "/^$(date --date='1 minutes ago' '+%b %_d %H:%M')/,\$p" /var/log/syslog | grep "DEMON" >> $SCRIPTPATH/syslog.txt
echo "Closing syslog..."
echo " "

rm -rf FolderA
rm -rf FolderB

