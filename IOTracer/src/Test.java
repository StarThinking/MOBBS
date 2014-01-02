
public class Test extends Thread{	
	public static void main(String[] args) {
		IOTracer tracer = new IOTracer();
		tracer.filter();
		while(true) {
			System.out.println("one minute start");
			tracer.readPerMinute();
			System.out.println("one minute end");
			try {
				sleep(1000*60);
			} catch(InterruptedException e) {
				e.printStackTrace();
			}
		}
	}

}
