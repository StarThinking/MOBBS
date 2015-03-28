ALL_O=Migrater.o thrift/StorageServer.o Storage.o
LIBRARY=-lthrift -lpthread -lmonitorservice -lstorageservice -lrados -lMobbsUtil

all: $(ALL_O)
	g++ $(ALL_O) -o Storage $(LIBRARY)

$(ALL_O):%.o:%.cc
	g++ -c $< -o $@

clean:
	rm -f *.o Storage gen-cpp/*.o
