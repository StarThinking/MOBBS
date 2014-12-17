ALL_OBJECT=gen-cpp/monitor_types.cpp gen-cpp/monitor_constants.cpp gen-cpp/MonitorService.cpp

all: gen-cpp/libmonitorservice.so

gen-cpp/libmonitorservice.so: $(ALL_OBJECT)
	mkdir -p monitor_service
	g++ -shared -fPIC $(ALL_OBJECT) -o $@
	cp gen-cpp/*.h monitor_service/
	cp gen-cpp/*.so monitor_service/

install:
	cp *_service/*.so /usr/lib/
	cp -r monitor_service ../../ceph-0.72/src/librbd/MOBBS/thrift
	cp -r monitor_service ../../storage/thrift

clean:
	rm -rf monitor_service
	cd gen-cpp; \
	rm -f *.o *.so *.a
