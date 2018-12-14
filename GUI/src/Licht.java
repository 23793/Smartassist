package GUI.src;

import java.awt.Point;

public class Licht {

	private boolean lichtStatus = false;
	private Point lichtPoint;

	public Licht(Point p) {
		lichtPoint = p;
	}

	public void toggle() {
		lichtStatus = !lichtStatus;

		if (lichtStatus) {
			System.out.println("Licht an");
		} else {
			System.out.println("Licht aus");

		}
	}

	public void setLichtStatus(boolean state) {
		lichtStatus = state;
	}

	public boolean getLichtStatus() {
		return lichtStatus;
	}

	public void setLichtPoint(Point p) {
		lichtPoint = p;
	}

	public Point getLichtPoint() {
		return lichtPoint;
	}

}
