package GUI.src;

public class Raum {

	private double position_x; // von links aus
	private double position_y; // von oben aus
	private int RaumID;
	private double height;
	private double width;

	private Modul modul = null;
	private Licht licht = null;
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
	public Raum(int id, Modul modul, Licht licht) {
		this.RaumID = id;
		this.modul = modul;
		this.licht = licht;
	}

	public Raum(int id, Modul modul){
		this.RaumID = id;
		this.modul = modul;
	}

	public Raum() {

	}

	public void lightswitch(int lightswitch ) {
		if (lightswitch == 1) {
			this.licht.toggle();
		}
	}

	public void setposition_x(double pos_x) {
		position_x = pos_x;
	}

	public double getposition_x() {
		return position_x;
	}

	public void setpositiony(double pos_y) {
		position_y = pos_y;
	}

	public double getposition_y() {
		return position_y;
	}

	public void setRaumID(int id) {
		RaumID = id;
	}

	public int getID() {
		return RaumID;
	}

	public void setheight(double h) {
		height = h;
	}

	public double getheight() {
		return height;
	}

	public void setwidth(double w) {
		width = w;
	}

	public double getwidth() {
		return width;
	}

	public boolean getState () {
		if (this.modul == null) {
			return false;
		} else {
			return true;
		}
	}

}
