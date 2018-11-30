package GUI.src;

import jssc.*;

public class Schnittstelle {

	private static String[] portNames = SerialPortList.getPortNames();

	public void init() {
		for (String p : portNames) {
			System.out.println(p);
		}
	}

	public String receive() {
		String x = null;

		return x;
	}

	public void send() {

	}
}
