package GUI.src;


public class Raum {

	private float position_x; //von links aus
	private float position_y; //von oben aus
	private int RaumID;
	private float height;
	private float width;
	private boolean Raumstatus;  //aktiv/inaktiv

	Modul modul = new Modul();
	Licht licht = new Licht();
	Klima klima = new Klima();

	public void setposition_x(float pos_x) {
		position_x = pos_x;
	}

	public float getposition_x() {
		return position_x;
	}

	public void setpositiony(float pos_y) {
		position_y = pos_y;
	}

	public float getposition_y() {
		return position_y;
	}

	public void setRaumID(int id) {
		RaumID = id;
	}

	public int getID() {
		return RaumID;
	}

	public void setheight(float h) {
		height = h;
	}

	public float getheight() {
		return height;
	}

	public void setwidth(float w) {
		width = w;
	}

	public float getwidth() {
		return width;
	}

	public void setRaumstatus(boolean state) {
		Raumstatus = state;
	}

	public boolean getRaumstatus() {
		return Raumstatus;
	}

}
