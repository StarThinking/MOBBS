ALL_OBJECT=gen-cpp/storage_types.cpp gen-cpp/storage_constants.cpp gen-cpp/StorageService.cpp

all: gen-cpp/libstorageservice.so

gen-cpp/libstorageservice.so: $(ALL_OBJECT)
	mkdir -p storage_service
	g++ -shared -fPIC $(ALL_OBJECT) -o $@
	cp gen-cpp/*.h storage_service/
	cp gen-cpp/*.so storage_service/

install:
	cp *_service/*.so /usr/lib/
	cp -r storage_service ../../monitor/thrift

clean:
	rm -rf storage_service
	cd gen-cpp; \
	rm -f *.o *.so *.a
