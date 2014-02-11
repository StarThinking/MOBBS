import java.util.List;


public class ExtentPerMinute {
	
	private long extentId;
	private int type;
	private int readNum;
	private int writeNum;
	
	public ExtentPerMinute(List<String> list) {
		this(Long.parseLong(list.get(0)), Integer.parseInt(list.get(1)), Integer.parseInt(list.get(2)), Integer.parseInt(list.get(3)));
	}
	
	public ExtentPerMinute(long extentId, int type, int readNum, int writeNum) {
		this.extentId = extentId;
		this.type = type;
		this.readNum = readNum;
		this.writeNum = writeNum;
	}
	
	public long getExtentId() {
		return extentId;
	}
	public void setExtentId(long extentId) {
		this.extentId = extentId;
	}
	public int getType() {
		return type;
	}
	public void setType(int type) {
		this.type = type;
	}
	public int getReadNum() {
		return readNum;
	}
	public void setReadNum(int readNum) {
		this.readNum = readNum;
	}
	public int getWriteNum() {
		return writeNum;
	}
	public void setWriteNum(int writeNum) {
		this.writeNum = writeNum;
	}

}
