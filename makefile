# Use -g flag to debug with gdb
CCFlags=-Wall -Werror -g -std=c++17
CC=g++

all: 35Manager.o skillList.o typeList.o raceList.o classList.o characterData.o groupData.o managerUI.o
	$(CC) $(CCFlags) -o 35Manager $^

35Manager.o: 35Manager.cc 35Manager.h
	$(CC) $(CCFlags) -c 35Manager.cc

skillList.o: skillList.cc
	$(CC) $(CCFlags) -c $^

typeList.o: typeList.cc
	$(CC) $(CCFlags) -c $^

raceList.o: raceList.cc
	$(CC) $(CCFlags) -c $^

classList.o: classList.cc
	$(CC) $(CCFlags) -c $^

characterData.o: characterData.cc
	$(CC) $(CCFlags) -c $^

groupData.o: groupData.cc
	$(CC) $(CCFlags) -c $^

managerUI.o: managerUI.cc
	$(CC) $(CCFlags) -c $^

#time:
#	sudo ntpdate ntp.ubuntu.com

clean:
	rm *.o
