package GUI.src;

public class Modul {

	private float temperatur = 20.00f;
	private int lichtwert = 200;
	private int ModulID;

	public Modul(int ModulID) {
		this.ModulID = ModulID;
	}

	public String temperaturanzeige(float tmp) {

		if (tmp < temperatur) {
			 System.out.println("Es ist gerade " + temperatur + " Grad, die	 Zieltemperatur ist " + tmp
			 + ". Es ist zu kalt." + " Es muss geheizt werden.");
			return "kalt";

		} else if (tmp > temperatur) {
			 System.out.println("Es ist " + temperatur + " Grad, die Zieltemperatur ist " + tmp + ". Es ist zu heiß."
			 + " Es muss gekühlt werden.");
			return "heiss";
		} else {
			 System.out.println("Es ist gerade " + temperatur + " Grad, die	 Zieltemperatur ist " + tmp
					+ ". Es ist perfekt.");
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
