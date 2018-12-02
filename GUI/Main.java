package GUI;

import GUI.src.*;

public class Main {
	private static Schnittstelle s = new Schnittstelle();
	private static Raum r1 = new Raum(), r2 = new Raum(), r3 = new Raum();

	public static void main(String[] args) {
		s.connect();

		s.send("Boi.");
		s.receive();

		s.close();
	}

	private void updateRooms() {
		String roomString;
		String roomAttributes[];
		Raum t1, t2, t3;

		s.connect();
		roomString = s.receive(); // Gets the room data String
		s.close();

		roomAttributes = roomString.split(";");

		// "id1;temp1;light1;switch1;id2;temp2;light2;switch2;id3;temp3;light3;switch3;

		// Creating temp rooms
		t1 = new Raum(Integer.parseInt(roomAttributes[0]), Float.parseFloat(roomAttributes[1]),
				Integer.parseInt(roomAttributes[2]), Integer.parseInt(roomAttributes[3]));
		t2 = new Raum(Integer.parseInt(roomAttributes[4]), Float.parseFloat(roomAttributes[5]),
				Integer.parseInt(roomAttributes[6]), Integer.parseInt(roomAttributes[7]));
		t3 = new Raum(Integer.parseInt(roomAttributes[8]), Float.parseFloat(roomAttributes[9]),
				Integer.parseInt(roomAttributes[10]), Integer.parseInt(roomAttributes[11]));

		// Muss hier noch eine Logik hin?
		r1 = t1;
		r2 = t2;
		r3 = t3;
	}
}

/*
 * GUI über Thread laufen lassen
 */
