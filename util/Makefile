ALL_OBJECT=ThreadPool.cc ClusterUtil.cc

all: libMobbsUtil.so

libMobbsUtil.so: $(ALL_OBJECT)
	g++ -shared -fPIC $(ALL_OBJECT) -o libMobbsUtil.so

install:
	cp *.so /usr/lib
	mkdir -p ../ceph-0.72/src/librbd/MOBBS/mobbs_util/
	cp *.h ../ceph-0.72/src/librbd/MOBBS/mobbs_util/
	mkdir -p ../storage/mobbs_util/
	cp *.h ../storage/mobbs_util/
	mkdir -p ../monitor/mobbs_util/
	cp *.h ../monitor/mobbs_util/

clean:
	rm -f *.o *.a *.so
