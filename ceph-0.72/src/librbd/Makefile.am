librbd_la_SOURCES = \
	librbd/librbd.cc \
	librbd/AioCompletion.cc \
	librbd/AioRequest.cc \
	librbd/ImageCtx.cc \
	librbd/internal.cc \
	librbd/LibrbdWriteback.cc \
	librbd/WatchCtx.cc \
	librbd/MOBBS/MOBBS.c \
	librbd/MOBBS/Migrater.cc \
	librbd/MOBBS/Analyzer.cc \
	librbd/MOBBS/ConfigParser.cc \
	librbd/MOBBS/Mapper.cc \
	librbd/MOBBS/Gather.cc \
	librbd/MOBBS/thrift/ClientServer.cc \
	librbd/MOBBS/thrift/gen-cpp/ClientService.cpp
librbd_la_LIBADD = \
	$(LIBRADOS) $(LIBOSDC) \
	libcls_rbd_client.la libcls_lock_client.la \
	-lthrift -lmonitorservice -lMobbsUtil\
	$(PTHREAD_LIBS) $(EXTRALIBS)
librbd_la_LDFLAGS = ${AM_LDFLAGS} -version-info 1:0:0 -export-symbols-regex '^rbd_.*'
lib_LTLIBRARIES += librbd.la

noinst_HEADERS += \
	librbd/AioCompletion.h \
	librbd/AioRequest.h \
	librbd/ImageCtx.h \
	librbd/internal.h \
	librbd/LibrbdWriteback.h \
	librbd/parent_types.h \
	librbd/SnapInfo.h \
	librbd/WatchCtx.h \
	librbd/MOBBS/MOBBS.h \
	librbd/MOBBS/Migrater.h \
	librbd/MOBBS/Analyzer.h \
	librbd/MOBBS/ConfigParser.h \
	librbd/MOBBS/Mapper.h \
	librbd/MOBBS/Gather.h \
	librbd/MOBBS/thrift/ClientServer.h \
	librbd/MOBBS/thrift/gen-cpp/ClientService.h
