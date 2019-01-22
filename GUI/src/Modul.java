package GUI.src;

/**
 * The Modul class contains the current light and temperature data of the module
 * in a room, it is updated with data received from the WSN.
 * 
 * @author MinhMax
 *
 */
public class Modul {

	private float temperatur = 20.00f;
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
	 * @return A String "heiß", "kalt", or "perfekt" used by the
	 *         setImageAndLabel() function in the Klima class
	 */
	public String temperaturanzeige(float tmp) {

		if (tmp < temperatur - 1.5) {
			// System.out.println("Es ist gerade " + temperatur + " Grad, die
			// Zieltemperatur ist " + tmp
			// + ". Es ist zu kalt." + " Es muss geheizt werden.");
			return "kalt";

		} else if (tmp > temperatur + 1.5) {
			// System.out.println("Es ist " + temperatur + " Grad, die
			// Zieltemperatur ist " + tmp + ". Es ist zu heiß."
			// + " Es muss gekühlt werden.");
			return "heiss";
		} else {
			// System.out.println("Es ist gerade " + temperatur + " Grad, die
			// Zieltemperatur ist " + tmp
			// + ". Es ist perfekt.");
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
