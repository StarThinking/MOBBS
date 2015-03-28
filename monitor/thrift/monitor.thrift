namespace cpp monitor

struct ExtentInfo {
  1:string m_eid,
  2:i32 m_pool,
  3:double m_rio,
  4:double m_wio,
}

struct ClientInfo {
  1:map<string, ExtentInfo> m_extents,
  2:string m_ip,
}

service MonitorService {
	void finish_lock(1:string eid),
	void report_client_info(1:ClientInfo ci),
	void finish_migration(1:string eid);
}
