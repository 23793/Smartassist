
public class Modul {

	private float temperatur;
	private int lichtwert;
	private int ModulID;


	Temppop tempsettings = new Temppop();
	Lichtpop lichtsettings = new Lichtpop();

/*	public void heisskaltanzeige(float temp) {

		if(temp < tempsettings.get_temp_zielwert()) {
			System.out.println("zu kalt");
		}
		else if (temp > tempsettings.get_temp_zielwert()) {
			System.out.println("zu heiß");
		} else {
			System.out.println("perfekte Raumtemperatur");
		}

	}*/

	public void auto() {  //einteilen in einstellung licht und einstellung temp .. vergessen

			tempsettings.set_temp_zielwert(22.00f);
			lichtsettings.set_licht_zielwert(2);
			System.out.println("Automatische Einstellung: ");
			System.out.println("Zieltemperatur = " +tempsettings.get_temp_zielwert());
			System.out.println("Ziellichtwert = " +lichtsettings.get_licht_zielwert());


	}

	public void manuell(int licht, float temp) {  //einteilen in einstellung licht und einstellung temp .. vergessen

		System.out.println("Manuelle Einstellung: ");
		tempsettings.set_temp_zielwert(temp);
		lichtsettings.set_licht_zielwert(licht);
		System.out.println("Zieltemperatur = " +tempsettings.get_temp_zielwert());
		System.out.println("Ziellichtwert = " +lichtsettings.get_licht_zielwert());

	}


	public void temperaturregulierung(float tmp) {

		if(tmp < tempsettings.get_temp_zielwert()) {
			System.out.println("Es ist gerade " + tmp+" Grad, die Zieltemperatur ist "
					+ tempsettings.get_temp_zielwert() +". Es ist zu kalt."+" Es muss geheizt werden (heat)"); // wird zum modul gesendet

		} else if (tmp > tempsettings.get_temp_zielwert()) {
			System.out.println("Es ist " + tmp + " Grad, die Zieltemperatur ist "
					+ tempsettings.get_temp_zielwert() +". Es ist zu heiß."+ " Es muss gekühlt werden (AC)");
		} else {
			System.out.println("Perfekte Raumemperatur.");
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
