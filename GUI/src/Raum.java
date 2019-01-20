package GUI.src;

import java.awt.Point;
import java.awt.Rectangle;

/**
 * The Raum class contains all the variables for a room (such as Licht, Modul
 * and Klima), as well as a constructor.
 * 
 * @author MinhMax
 *
 */
public class Raum {

	private double position_x;
	private double position_y; // Lower right point
	private int RaumID;
	private Rectangle rect;

	private Modul modul = null;
	private Licht licht = null;
	private Klima klima = null;

	/**
	 * Creates a room with the given parameters
	 * 
	 * @param id
	 *            the ID of the room
	 * @param x
	 *            the X position of the room
	 * @param y
	 *            the Y position of the room
	 * @param rectangle
	 *            the rectangle that belongs to the room
	 */
	public Raum(int id, double x, double y, Rectangle rectangle) {
		this.RaumID = id;
		this.position_x = x;
		this.position_y = y;
		this.rect = rectangle;
	}

	public Raum() {

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

	/**
	 * @return the module
	 */
	public Modul getModul() {
		return modul;
	}

	/**
	 * @param modul
	 *            the module to add to the room
	 */
	public void setModul(Modul modul) {
		this.modul = modul;
		try {
			this.klima = new Klima(new Point((int) (position_x), (int) (position_y)), this);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	/**
	 * @return the light
	 */
	public Licht getLicht() {
		return licht;
	}

	/**
	 * @param licht
	 *            the light to add to the room
	 */
	public void setLicht(Licht licht) {
		this.licht = licht;
	}

	/**
	 * @return the climate control of the room
	 */
	public Klima getKlima() {
		return klima;
	}

}
