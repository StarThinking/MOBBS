namespace cpp librbd

service ClientService {
	void begin_migration(1:string eid, 2:i32 from, 3:i32 to),
	void finish_migration(1:string eid, 2:i32 from, 3:i32 to),
}
