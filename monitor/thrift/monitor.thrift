namespace cpp monitor

service MonitorServer {
	i32 lock_and_get_pool(1:string eid),
	void release_lock(1:string eid)
}
