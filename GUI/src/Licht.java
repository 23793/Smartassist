package GUI.src;


public class Licht {

	private boolean licht_status;
	private float licht_position_x;
	private float licht_position_y;

/*	public void toggle(boolean s) {
		if(s == true) {
			setlicht_status(false);
			System.out.println("Off");
		}
		else if(s == false) {
			setlicht_status(true);
			System.out.println("On");
		}

	}*/

	public void toggle() {
		licht_status = !licht_status;

		if(licht_status) {
			System.out.println("Licht an");
		} else {
			System.out.println("Licht aus");

		}
	}

	public void setlicht_status(boolean state) {
		licht_status = state;
	}

	public boolean getlicht_status() {
		return licht_status;
	}

	public void set_licht_position_x(float x) {
		licht_position_x = x;
	}

	public float get_licht_position_x() {
		return licht_position_x;
	}

	public void set_licht_position_y(float y) {
		licht_position_y = y;
	}

	public float get_licht_position_y() {
		return licht_position_y;
	}

}
