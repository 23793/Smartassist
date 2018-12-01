package GUI;

import GUI.src.*;

public class Main {
	private static Schnittstelle s = new Schnittstelle();

	public static void main(String[] args) {
		s.connect();

		s.send("Boi.");
		s.receive();

		s.close();
	}

	private void updateRooms() {
		String roomString;
		String roomAttributes[];

		s.connect();
		roomString = s.receive(); // Gets the room data String
		s.close();

		roomAttributes = roomString.split(";");
		
		// Check if rooms exist
	}
}

/*
 * GUI über Thread laufen lassen
 */
