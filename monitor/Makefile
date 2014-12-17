all: Monitor.o Analyzer.o thrift/MonitorServer.o MOBBS.o
	g++ Monitor.o Analyzer.o thrift/MonitorServer.o MOBBS.o -o Monitor -lpthread -lthrift -lmonitorservice -lstorageservice

MOBBS.o: MOBBS.h
	gcc -c MOBBS.c -o MOBBS.o

Monitor.o: Analyzer.o thrift/MonitorServer.o
	g++ -c Monitor.cc -o Monitor.o

thrift/MonitorServer.o: Analyzer.o
	cd thrift; \
	g++ -c MonitorServer.cc -o MonitorServer.o

Analyzer.o: Analyzer.h Analyzer.cc
	g++ -c Analyzer.cc -o Analyzer.o

clean:
	rm -f *.o Monitor
	cd thrift; \
	rm -f *.o

