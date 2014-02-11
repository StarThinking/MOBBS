import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;


public class IOTracer {

	private long skippedCharNum = 0;
	private File file = new File("/usr/local/libvirt/var/log/libvirt/qemu/Ubuntu.log");
	private List<ExtentPerMinute> extentPerMinutelist = new ArrayList<ExtentPerMinute>();
	
	public void filter() {
		try {
			BufferedReader bufferedreader = new BufferedReader(new FileReader(file));
			String lineContent;
			bufferedreader.skip(skippedCharNum);
			while((lineContent = bufferedreader.readLine()) != null) {
				if(!lineContent.contains("1388")) {	
					int num = lineContent.getBytes().length;
					skippedCharNum += num;
					skippedCharNum ++;
				} else {
					break;	
				}
			}
			bufferedreader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}

	public void readPerMinute() {
		try {
			List<List<String>> recordList = new ArrayList<List<String>>();
			boolean isFirst = true;
			long time = 0;
			long startTime = 0;
			BufferedReader bufferedreader = new BufferedReader(new FileReader(file));
			String lineContent;
			bufferedreader.skip(skippedCharNum);
			while((lineContent = bufferedreader.readLine()) != null) {
				StringTokenizer st = new StringTokenizer(lineContent, " ");
				List<String> fieldList = new ArrayList<String>();
				String field;
				while(st.hasMoreTokens()) {
					field = st.nextToken();
					fieldList.add(field);
				}
				String timeStr = fieldList.remove(0);
				if(!timeStr.contains("1388")) {
					continue;
				}
				time = Long.parseLong(timeStr);
				recordList.add(fieldList);
				if(isFirst) {
					isFirst = false;
					startTime = time;
				}
				if(time <= startTime + 59) {	
					int num = lineContent.getBytes().length;
					skippedCharNum += num;
					skippedCharNum ++;
				//	System.out.println(lineContent);
				} else {
					break;	
				}
			}
			System.out.println("record num in this minute is " + recordList.size());
			bufferedreader.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		} 
	}

}
