namespace cpp monitor

struct ExtentInfo {
  1:string m_eid,
  2:i32 m_pool,
  3:i32 m_rio,
  4:i32 m_wio,
}

struct ClientInfo {
  1:list<ExtentInfo> m_extents,
  2:string m_ip,
}

service MonitorService {
	void finish_lock(1:string eid),
	void report_client_info(1:ClientInfo ci),
}
