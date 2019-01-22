package GUI.src;

/**
 * The Modul class contains the current light and temperature data of the module
 * in a room, it is updated with data received from the WSN.
 * 
 * @author MinhMax
 *
 */
public class Modul {

	private float temperatur = 00.00f;
	private int lichtwert = 200;
	private int ModulID;

	public Modul(int ModulID) {
		this.ModulID = ModulID;
	}

	/**
	 * Compares a given temperature to the current room temperature for
	 * determining the temperature display icon. The perfect temperature range
	 * is +-1.5 degrees from the desired temperature target value.
	 * 
	 * @param tmp
	 *            the temperature to be compared to the current room temperature
	 * @return A String "heiﬂ", "kalt", or "perfekt" used by the
	 *         setImageAndLabel() function in the Klima class
	 */
	public String temperaturanzeige(float tmp) {

		if (tmp < temperatur - 1.5) {
			return "kalt";
		} else if (tmp > temperatur + 1.5) {
			return "heiss";
		} else {
			return "perfekt";
		}

	}

	public void settemperatur(float temp) {
		temperatur = temp;
	}

	public float gettemperatur() {
		return temperatur;
	}

	public void setlichtwert(int wert) {
		lichtwert = wert;
	}

	public int getlichtwert() {
		return lichtwert;
	}

	public void setModulID(int id) {
		ModulID = id;
	}

	public int getModulID() {
		return ModulID;

	}

}
