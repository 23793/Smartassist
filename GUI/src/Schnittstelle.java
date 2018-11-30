import jssc.*;

public class Schnittstelle {

	private static String[] portNames = SerialPortList.getPortNames();

	public void init() {
		for (String p : portNames) {
			system.out.println(p);
		}

		return true;
	}

	public String receive() {
		String x;

		return x;
	}

	public void send() {

		return true;
	}
}
