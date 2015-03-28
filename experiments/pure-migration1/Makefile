ALL_OBJECT=Client.o Migrater.o 
THRIFT_OBJECT=gen-cpp/ClientService.o gen-cpp/MigraterService.o
LIBRARY=-lpthread -lthrift -lrados

all: $(ALL_OBJECT) $(THRIFT_OBJECT)
	g++ main_c.cc $(ALL_OBJECT) $(THRIFT_OBJECT) -o main_c $(LIBRARY)
	g++ main_m.cc $(ALL_OBJECT) $(THRIFT_OBJECT) -o main_m $(LIBRARY)

$(ALL_OBJECT): %.o: %.cc %.h
	g++ -c $< -o $@

$(THRIFT_OBJECT): %.o: %.cpp %.h
	g++ -c $< -o $@

clean:
	rm -f *.o gen-cpp/*.o main_c main_m
