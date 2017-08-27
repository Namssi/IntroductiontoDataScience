all: DES.o Event.o Scheduler.o Process.o
	g++ -o DES DES.o Event.o Process.o Scheduler.o
DES.o: DES.cpp Event.h Scheduler.h Process.h
	g++ -c DES.cpp
Event.o: Event.cpp Event.h Process.h
	g++ -c Event.cpp
Scheduler.o: Scheduler.cpp Scheduler.h Process.h
	g++ -c Scheduler.cpp
Process.o: Process.cpp Process.h
	g++ -c Process.cpp
