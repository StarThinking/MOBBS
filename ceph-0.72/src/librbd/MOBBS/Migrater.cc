// written by masixiang

#include "common/ceph_context.h"
#include "common/dout.h"
#include "common/errno.h"
#include "librbd/ImageCtx.h"
#include "librbd/internal.h"
#include "librbd/WatchCtx.h"

#include <ctime>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dlfcn.h>

#include "librbd/MOBBS/Migrater.h"

namespace librbd {

  Migrater::Migrater(ImageCtx *ictx) :
    ictx(ictx)
  {}

  void Migrater::migrate_to_default_pool()
  {
    	if(TAKE_LOG_MIGRATER)
    	{
    		take_log("migrate all extents back to DEFAULT POOL");
    	}
  	int count = 0;
	int next = 0;
  	for(std::map<std::string, int>::iterator it = ictx->m_mapper->m_extent_map.begin(); it != ictx->m_mapper->m_extent_map.end(); it ++) 
	{
		std::string eid = it->first;
		int pool = it->second;
		if(pool != DEFAULT_POOL)
		{
			do_concurrent_migrate(eid, pool, DEFAULT_POOL);
		}
		count ++;
		int ratio = count * 100 / ictx->m_mapper->m_extent_map.size();
		if(ratio == next)
		{
    			if(TAKE_LOG_MIGRATER)
    			{	
    				char my_log[100];
    				sprintf(my_log, "migrating process: %d", ratio);
    				take_log(my_log);
    			}
			next += 10;
			
		}

	}
  }

  void Migrater::do_concurrent_migrate(std::string extent_id, int from_pool, int to_pool)
  {
    if(TAKE_LOG_MIGRATER)
    {
    	char my_log[100];
    	sprintf(my_log, "migrating: %s from:%d to:%d", extent_id.c_str(), from_pool, to_pool);
    	take_log(my_log);
    }
    struct timeval tv_begin, tv_end;
    //struct timeval tv_begin1, tv_end1;
    long time_used;
    gettimeofday(&tv_begin, NULL);
    int ret = 0;
   
    librados::IoCtx io_ctx_from = ictx->data_ctx[from_pool];
    librados::IoCtx io_ctx_to = ictx->data_ctx[to_pool];

    // lock extent map
    ictx->m_mapper->lock_extent(extent_id);

    // reading
    librados::bufferlist read_buf;
    ret = io_ctx_from.read(extent_id, read_buf, OBJECT_SIZE, 0);
    if( ret < 0 ) 
    {
    	std::cout << "fail to read" << std::endl;
	exit(ret);
    }

    // writing
    ret = io_ctx_to.write_full(extent_id, read_buf);
    if( ret < 0 ) 
    {
    	std::cout << "fail to write" << std::endl;
	exit(ret);
    }

    // update extent map
    ictx->m_mapper->set_pool(extent_id, to_pool);

    // unlock
    ictx->m_mapper->release_extent(extent_id);

    // remove
    io_ctx_from.remove(extent_id);
    if( ret < 0 ) 
    {
    	std::cout << "fail to remove" << std::endl;
	exit(ret);
    }

    gettimeofday(&tv_end, NULL);
    time_used = 1000000 * (tv_end.tv_sec - tv_begin.tv_sec) + (tv_end.tv_usec - tv_begin.tv_usec); //us
    time_used /= 1000;
    if(TAKE_LOG_MIGRATER)
    {
    	char my_log4[100];
    	sprintf(my_log4, "total time used: %ld", time_used);
    	take_log(my_log4);
    }


  }

  bool Migrater::get_migrating()
  {
    return migrating;
  }

  void Migrater::set_migrating(bool _migrating)
  {
    migrating = _migrating;
  }

}
