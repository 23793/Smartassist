package GUI.src;


public class Modul {

	private float temperatur;
	private int lichtwert;
	private int ModulID;


	Temppop tempsettings = new Temppop();
	Lichtpop lichtsettings = new Lichtpop();

	public Modul( int ModulID )
	{
		this.ModulID = ModulID;
	}

	public void einstellung_licht(int modus, int ziel_licht) {  //einteilen in einstellung licht und einstellung temp .. vergessen

		//manuell
		if(modus == 0) {
			lichtsettings.set_licht_zielwert(ziel_licht);
			System.out.println("Wir sind im manuellen modus: " +"Ziellichtwert: "
			+lichtsettings.get_licht_zielwert());
		} else if (modus == 1) { //automatisch
			lichtsettings.set_licht_zielwert(2);
			System.out.println("Wir sind im automatischen modus: " +"Ziellichtwert: "
					+lichtsettings.get_licht_zielwert());
		}

	}

	public void einstellung_temp(int modus, float ziel_temp) {  //einteilen in einstellung licht und einstellung temp .. vergessen

		if(modus == 0 ) {
			tempsettings.set_temp_zielwert(ziel_temp);
			System.out.println("Wir sind im manuellen modus: " +"Zieltempwert: "
					+tempsettings.get_temp_zielwert());
		} else if (modus == 1) {
			tempsettings.set_temp_zielwert(22.00f);
			System.out.println("Wir sind im automatischen modus: " +"Zieltempwert: "
					+tempsettings.get_temp_zielwert());
		}

	}


	public String temperaturanzeige(float tmp) {

		if(tmp < tempsettings.get_temp_zielwert()) {
			System.out.println("Es ist gerade " + tmp+" Grad, die Zieltemperatur ist "
					+ tempsettings.get_temp_zielwert() +". Es ist zu kalt."+" Es muss geheizt werden."); // wird zum modul gesendet
			return "kalt";

		} else if (tmp > tempsettings.get_temp_zielwert()) {
			System.out.println("Es ist " + tmp + " Grad, die Zieltemperatur ist "
					+ tempsettings.get_temp_zielwert() +". Es ist zu heiß."+ " Es muss gekühlt werden.");
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
