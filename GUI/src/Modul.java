package GUI.src;

public class Modul {

	private float temperatur = 20.00f;
	private int lichtwert = 200;
	private int ModulID;

	public Modul(int ModulID) {
		this.ModulID = ModulID;
	}

	// Einteilung in einstellung Licht und einstellung Temp.. vergessen
	public void einstellung_licht(int modus, int ziel_licht) {
		// manuell
		if (modus == 0) {
			System.out.println("Wir sind im manuellen modus");
		} else if (modus == 1) { // automatisch
			System.out.println("Wir sind im automatischen modus");
		}
	}

	// Einteilen in einstellung licht und einstellung temp.. vergessen
	public void einstellung_temp(int modus, float ziel_temp) {
		if (modus == 0) {
			System.out.println("Wir sind im manuellen modus");
		} else if (modus == 1) {
			System.out.println("Wir sind im automatischen modus");
		}
	}

	public String temperaturanzeige(float tmp) {

		if (tmp < temperatur) {
			System.out.println("Es ist gerade " + temperatur + " Grad, die Zieltemperatur ist "
			// Wird zum modul gesendet
					+ tmp + ". Es ist zu kalt." + " Es muss geheizt werden.");
			return "kalt";

		} else if (tmp > temperatur) {
			System.out.println("Es ist " + temperatur + " Grad, die Zieltemperatur ist " + tmp + ". Es ist zu heiß."
					+ " Es muss gekühlt werden.");
			return "heiss";
		} else {
			System.out.println("Perfekte Raumemperatur.");
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
