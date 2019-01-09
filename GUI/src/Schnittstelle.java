package GUI.src;

import jssc.*;

public class Schnittstelle {

	private static SerialPort serialPort = null;
	private static String[] portNames = SerialPortList.getPortNames();

	/**
	 * Initializes the connection to the right port where the Controller is
	 * connected via a handshake.
	 * 
	 * @see SerialPort
	 */
	public void connect() {
		if (serialPort == null) { // Port has to be found for the first time
			SerialPort tempPort;
			String s = null;
			for (String p : portNames) { // Loop looking for the right port
				tempPort = new SerialPort(p);
//				System.out.println(p);
				try {
					tempPort.openPort();
					tempPort.setParams(SerialPort.BAUDRATE_38400, SerialPort.DATABITS_8, SerialPort.STOPBITS_1,
							SerialPort.PARITY_NONE);
					try {
						tempPort.writeString("Hallo!E"); // Handshake
						s = new String(tempPort.readBytes(19, 50));
					} catch (SerialPortTimeoutException e) {
						// e.printStackTrace(); // DO NOTHING
					}
					tempPort.closePort();
				} catch (SerialPortException e) {
					e.printStackTrace();
				}
				if (s != null && s.matches(".*Hallo.*")) { // Regular expression
					serialPort = tempPort;
					break;
				}
			}

			if (serialPort != null) {
				try {
					System.out.println("Connecting to " + serialPort.getPortName() + "...");
					serialPort = new SerialPort(serialPort.getPortName());
					serialPort.openPort();
					System.out.println("Connected!");
				} catch (SerialPortException e) {
					e.printStackTrace();
				}
			} else {
				// System.out.println("Kein passendes Gerät gefunden!"); //DO
				// NOTHING
			}
		} else { // Port already found
			try {
				System.out.println("Connecting to " + serialPort.getPortName() + "...");
				serialPort.openPort();
				System.out.println("Connected!");
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
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
		if (serialPort != null) {
			String x = null;
			try {
				if (!serialPort.isOpened()) {
					serialPort.openPort();
				}

				System.out.println("Waiting for data from the WSN...");
				x = new String(serialPort.readBytes(19)); // Reads the 36 Bytes
															// of
															// sensor and status
															// data from the WSN
				System.out.println("'" + x + "' Received!");
			} catch (SerialPortException e) {
				System.out.println("Could not receive the data from the WSN!");
				e.printStackTrace();
			}
			return x;
		} else {
			System.out.println("Kein Port verbunden!");
			return null;
		}
	}

	/**
	 * Sends data to the WSN via the connected controller on the connected port.
	 * 
	 * @param data
	 *            the String of data to send to the WSN
	 */
	public void send(String data) {
		if (serialPort != null) {
			try {
				if (!serialPort.isOpened()) {
					serialPort.openPort();
				}
				System.out.println("Sending data to the WSN...");
				serialPort.writeString(data); // Sends the data String to the
												// connected port
				System.out.println("Data sent!");
			} catch (SerialPortException e) {
				System.out.println("Could not send the data to the WSN!");
				e.printStackTrace();
			}
		} else {
			System.out.println("Kein Port verbunden!");
		}
	}

	/**
	 * Closes the connection to the connected port.
	 * 
	 * @see SerialPort
	 */
	public void close() {
		if (serialPort != null && serialPort.isOpened()) {
			System.out.println("Closing connection...");
			try {
				serialPort.closePort();
			} catch (SerialPortException e) {
				e.printStackTrace();
			}
			System.out.println("Connection closed!");
		} else {
			System.out.println("Kein Port verbunden!");
		}
	}

	public static SerialPort getSerialPort() {
		return serialPort;
	}
}