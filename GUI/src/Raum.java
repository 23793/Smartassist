package GUI.src;

import java.awt.Rectangle;

/**
 * Die Raumklasse ist f�r die Logik und Funktionalit�ten von R�umen zust�ndig.
 * 
 * @author Minh
 *
 */
public class Raum {

	private double position_x;
	private double position_y; // Punkt rechts unten
	private int RaumID;
	private Rectangle rect;

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
	 *            boolean der angibt, ob der Lichtschalter bet�tigt wurde
	 */

	public Raum(int id, double x, double y, Rectangle rectangle) {
		this.RaumID = id;
		this.position_x = x;
		this.position_y = y;
		this.rect = rectangle;
	}

	public Raum() {

	}

	public void lightswitch(int lightswitch) {
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

	public Rectangle getRect() {
		return rect;
	}

	public void setRect(Rectangle rect) {
		this.rect = rect;
	}

	public boolean getState() {
		if (this.modul == null) {
			return false;
		} else {
			return true;
		}
	}

}
