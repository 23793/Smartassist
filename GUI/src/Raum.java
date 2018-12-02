package GUI.src;

public class Raum {

	private float position_x; // von links aus
	private float position_y; // von oben aus
	private int RaumID;
	private float height;
	private float width;
	private boolean Raumstatus; // aktiv/inaktiv

	private Modul modul = new Modul();
	private Licht licht = new Licht();
	private Klima klima = new Klima();

	/**
	 * Erstellt ein Raumobjekt mit den gegebenen Parametern.
	 * 
	 * @param id
	 *            die ID des Raums
	 * @param temp
	 *            die aktuelle Temperatur im Raum
	 * @param lichtwert
	 *            der aktuelle Lichtwert im Raum
	 * @param lightswitch
	 *            boolean der angibt, ob der Lichtschalter betätigt wurde
	 */
	public Raum(int id, float temp, int lichtwert, int lightswitch) {
		this.RaumID = id;
		this.modul.settemperatur(temp);
		this.modul.setlichtwert(lichtwert);
		lightswitch(lightswitch);
	}

	public Raum() {

	}

	public void lightswitch(int lightswitch ) {
		if (lightswitch == 1) {
			this.licht.toggle();
		}
	}

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
