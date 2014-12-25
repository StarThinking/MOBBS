ALL_OBJECT=Client.o ObjectGenerater.o
LIBRARY=-lrados

all: $(ALL_OBJECT)
	g++ main.cc $(ALL_OBJECT) -o main $(LIBRARY)

$(ALL_OBJECT): %.o: %.cc %.h
	g++ -c $< -o $@

clean:
	rm -f *.o main
