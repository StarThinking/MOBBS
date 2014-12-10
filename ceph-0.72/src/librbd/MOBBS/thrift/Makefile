all: libclientservice.so

libclientservice.so:
	mkdir -p client_service
	cd gen-cpp; \
	g++ -shared -fPIC ClientService.cpp -o libclientservice.so; \
	cp *.h ../client_service; \
	cp *.so ../client_service

clean:
	rm -rf client_service
	cd gen-cpp; \
	rm -f *.so
