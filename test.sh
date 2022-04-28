#!/bin/sh

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
make reload
rm -rf syslog.txt
rm -rf valgrindlogs.txt
#sudo truncate -s 0 /var/log/syslog
echo "Starting test" 
echo "Test 1 - Simplest copying of files from FolderA to FolderB."
echo "Program will not use any extra parameters."
echo "Since the base waiting time is 300 seconds, SIGUSR1 will be sent after 5 seconds"

echo "Creating test files..."
rm -rf FolderA
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB
sleep 1s

echo "Creating test files... Done."
echo "Listing test files..."
tree FolderA
tree FolderB


echo "Running program..."
sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB
echo "1.."
sleep 1s
echo "2.."
sleep 1s
echo "3.."
sleep 1s
echo "4.."
sleep 1s
echo "5.."
sleep 1s
echo "Sending SIGUSR1 signal..."
pidprogramu="$(pidof ./syndi)"
kill -s SIGUSR1 $pidprogramu
sleep 1s

echo "Copying files... Done."
echo "Showing results..."
tree FolderA
tree FolderB

echo "Removing test files..."
rm -rf FolderA
rm -rf FolderB
echo "Removing test files... Done."
echo "Test 1 - Finished"
echo " "
echo " "

echo "Test 2 - Copying files from FolderA to FolderB with recursive option."
echo "Program will use -R option, making it recursive."
echo "Program will use -t 0 option, making it start instantly without waiting."
echo "Program will use -s 32000000, making it copy files smaller than 32MB bytes using copy/write, and files bigger than 32MB using mmap/write."

echo "Creating test files..."
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
rm -rf FolderB
mkdir FolderB

echo "Creating test files... Done."
echo "Listing test files..."
tree FolderA
tree FolderB

echo "Running program..."
sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -t 0 -s 32000000
sleep 1s

echo "Copying files... Done."
echo "Showing results..."
tree FolderA
tree FolderB

echo "Removing test files..."
rm -rf FolderA
rm -rf FolderB
echo "Removing test files... Done."

echo "Test 2 - Finished"
echo " "

echo "Test 3 - Copying files from FolderA to FolderB, with files in FolderB that do not exist in FolderA."
echo "Program will use -t 0 option, making it start instantly without waiting."
echo "Program will use -s 32000000, making it copy files smaller than 32MB bytes using copy/write, and files bigger than 32MB using mmap/write."

echo "Creating test files..."
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderB
mkdir FolderB/SubFolderB
dd if=/dev/zero of=FolderB/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/plik32MB.txt count=32 bs=1MB status=none
echo "Creating test files... Done."

echo "Listing test files..."
tree FolderA
tree FolderB

echo "Running program..."
sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -t 0 -s 32000000
sleep 1s

echo "Copying files... Done."
echo "Showing results..."
tree FolderA
tree FolderB

echo "Removing test files..."
rm -rf FolderA
rm -rf FolderB
echo "Removing test files... Done."
echo "Test 3 - Finished"
echo " "

echo "Test 4 - Copying files from FolderA to FolderB."
echo "FolderB will contain SubFolderB containing SubFolderC containing random files."
echo "SubFolderB does not exist in FolderA" 
echo "FolderA and FolderB will contain files with the same size and content, but different names."
echo "Program will use -R option, making it recursive."
echo "Program will use -t 0 option, making it start instantly without waiting."
echo "Program will use -s 32000000, making it copy files smaller than 32MB bytes using copy/write, and files bigger than 32MB using mmap/write."

echo "Creating test files..."
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
dd if=/dev/zero of=FolderA/plik16MB_A.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderB
mkdir FolderB/SubFolderB
mkdir FolderB/SubFolderB/SubFolderC
dd if=/dev/zero of=FolderB/plik16MB.txt count=16 bs=1MB status=none
dd if=/dev/zero of=FolderB/plik16MB_B.txt count=16 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/SubFolderC/plik8MB.txt count=8 bs=1MB status=none

echo "Creating test files... Done."
echo "Listing test files..."
tree FolderA
tree FolderB

echo "Running program..."
sleep 1s
./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -t 0 -s 32000000
sleep 1s

echo "Copying files... Done."
echo "Showing results..."
tree FolderA
tree FolderB

echo "Removing test files..."
rm -rf FolderA
rm -rf FolderB
echo "Removing test files... Done."
echo "Test 4 - Finished"
echo " "

echo "Test 5 - Valgrind"
echo "Program will use Valgrind to check for memory leaks."
echo "Program will use -R option, making it recursive."
echo "Program will use -t 0 option, making it start instantly without waiting."
echo "Program will use -s 32000000, making it copy files smaller than 32MB bytes using copy/write, and files bigger than 32MB using mmap/write."

echo "Creating test files..."
mkdir FolderA
dd if=/dev/zero of=FolderA/plik16MB.txt count=16 bs=1MB status=none
dd if=/dev/zero of=FolderA/plik16MBA.txt count=16 bs=1MB status=none
dd if=/dev/zero of=FolderA/plik16MBB.txt count=16 bs=1MB status=none
mkdir FolderA/SubFolderA
dd if=/dev/zero of=FolderA/SubFolderA/plik64MB.txt count=16 bs=4MB status=none
dd if=/dev/zero of=FolderA/SubFolderA/plik16MB.txt count=16 bs=1MB status=none
mkdir FolderB
mkdir FolderB/SubFolderB
mkdir FolderB/SubFolderB/SubFolderC
mkdir FolderB/SubFolderB/SubFolderC/SubFolderD
dd if=/dev/zero of=FolderB/SubFolderB/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/SubFolderC/plik8MB.txt count=8 bs=1MB status=none
dd if=/dev/zero of=FolderB/SubFolderB/SubFolderC/SubFolderD/plik8MB.txt count=8 bs=1MB status=none

echo "Creating test files... Done."
echo "Listing test files..."
tree FolderA
tree FolderB

echo "Running program..."
sleep 3s
valgrind --leak-check=yes --track-origins=yes --log-file=valgrindlogs.txt ./syndi $SCRIPTPATH/FolderA $SCRIPTPATH/FolderB -R -t 0 -s 32000000
sleep 3s

echo "Copying files... Done."
echo "Showing results..."
tree FolderA
tree FolderB

echo "Removing test files..."
rm -rf FolderA
rm -rf FolderB
echo "Removing test files... Done."
echo "Test 5 - Finished"
echo " "

echo "All tests finished."
echo "Valgrind logs can be found in valgrindlogs.txt"
echo "Syslogs can be found in syslogs.txt"
perl -MDate::Parse -ne 'print if/^(.{15})\s/&&str2time($1)>time-60' /var/log/syslog | grep "DEMON" >> $SCRIPTPATH/syslog.txt

