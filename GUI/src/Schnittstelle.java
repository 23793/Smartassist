package GUI.src;

import jssc.*;

public class Schnittstelle {

	private static SerialPort serialPort;
	private static String[] portNames = SerialPortList.getPortNames();

	/**
	 * Initializes the connection to a port.
	 * 
	 * @see SerialPort
	 */
	public void connect() {
		System.out.println("Available ports:");
		for (String p : portNames) {
			System.out.println(p); // Prints a list of all connected ports
		}

		System.out.println();
		System.out.println("Connecting to " + portNames[1] + "...");
		serialPort = new SerialPort(portNames[1]); // Connects to the second one
													// in the list (Works for
													// testing, finding the
													// right port dynamically
													// needs to be solved)
		try {
			serialPort.openPort();
			serialPort.setParams(SerialPort.BAUDRATE_38400, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
					SerialPort.PARITY_NONE);
			System.out.println("Connected!");
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
	}

	/**
	 * Retrieves data from the WSN via the connected controller on the connected
	 * port.
	 * <p>
	 * The data has the following format:
	 * [modulnr];[aktuelle_temperatur];[aktueller_lichtwert];[switch]; for each
	 * module.
	 * 
	 * @return a String containing all WSN data, separated by ";"
	 */
	public String receive() {
		String x = null;
		try {
			System.out.println("Waiting for data from the WSN...");
			x = new String(serialPort.readBytes(36)); // Reads the 36 Bytes of
														// sensor and status
														// data from the WSN
			System.out.println("'" + x + "' Received!");
		} catch (SerialPortException e) {
			System.out.println("Could not receive the data from the WSN!");
			e.printStackTrace();
		}
		return x;
	}

	/**
	 * Sends data to the WSN via the connected controller on the connected port.
	 * 
	 * @param data
	 *            the String of data to send to the WSN
	 */
	public void send(String data) {
		try {
			System.out.println("Sending data to the WSN...");
			serialPort.writeString(data); // Sends the data String to the
											// connected port
			System.out.println("Data sent!");
		} catch (SerialPortException e) {
			System.out.println("Could not send the data to the WSN!");
			e.printStackTrace();
		}
	}

	/**
	 * Closes the connection to the connected port.
	 * 
	 * @see SerialPort
	 */
	public void close() {
		System.out.println("Closing connection...");
		try {
			serialPort.closePort();
		} catch (SerialPortException e) {
			e.printStackTrace();
		}
		System.out.println("Connection closed!");
	}
}
