
public class Main {

	public static void main(String[] args) {


		String raum = "1;24.33f;1;switch;";  //3 Räume in einen String;
		//modulnr, aktuelle temp, lichtwert, switch oder no

		String gui ="1;3;25f;no;manuell"; //manu/auto
		//bekommen wir von der GUI

		String[] parts = raum.split(";");
		String modulnr = parts[0]; //modulnr
		Float temp_aktuell = Float.parseFloat(parts[1]); //aktuelle temp
		int licht_aktuell = Integer.parseInt(parts[2]);//lichtwert
		String wechsel = parts[3]; //kommando ob man switchen soll oder nicht


		String[] guidaten = gui.split(";");
		String modul = guidaten[0]; //modulnr
		int licht_ziel = Integer.parseInt(guidaten[1]);//lichtwert
		Float temp_ziel = Float.parseFloat(guidaten[2]); //aktuelle temp
		String modus = guidaten[4]; //kommando ob man switchen soll oder nicht



		String toggle = "switch";
		String manu = "manuell";
		String auto = "automatisch";

		//ids vergleichen von String gui -> nochmal ÜBERLEGEN
		String id="";
		if(modulnr.equals(modul)) {
			id = parts[0];
		}

		System.out.print(id);

		switch(id) {
		case "1":


			Raum zimmer1 = new Raum();
			zimmer1.modul.setModulID(1);

			/*Temperaturanzeige in der GUI mit aktuellen Temperaturwerten von den Modulen */
			zimmer1.modul.settemperatur(temp_aktuell); //aktuelle Temperatur von modulen 22.33
			zimmer1.modul.setlichtwert(licht_aktuell); //aktueller lichtwert bekommen wir vom zigbeemodul legen ihn fest

			System.out.println("Modul 1:");
			System.out.println("Aktuelle Temperatur: " +zimmer1.modul.gettemperatur());
			System.out.println("Aktueller Lichtwert: " +zimmer1.modul.getlichtwert());


			/*Wenn Lichtschalter am Home Automations gerät betätigt wird ODER
			 * wenn der Benutzer in der GUI das Licht ändert*/

			if(wechsel.equals(toggle)) {

			zimmer1.licht.toggle(); //parameter entweder vom Benutzer oder vom modul

			}


			/*Automatische einstellung (false = manuell, true = automatisch)
			 * Benutzer: Bei Automatisch muss int -> 0 und float -> 0 übergeben werden
			 * Bei manuell muss int -> LichtZielwert (1 = hell, 2 = mittel, 3 = dunkel) und
			 * float -> TemperaturZielwert übergeben werden*/
			zimmer1.modul.einstellung_licht(0, licht_ziel);
			zimmer1.modul.einstellung_temp(0, temp_ziel);

			/*Temperaturregulierung: aktuelle Temperatur bekommen wir von den Zigbeemodulen und vergleichen
			 * sie mit den Zielwerten, die bei "einstellung()" konfiguriert wurden */
			zimmer1.modul.temperaturanzeige(zimmer1.modul.gettemperatur()); // wir übergeben die aktuelle temp

			break;

		case "2":
			System.out.println("Wir befinden uns in Modul 2");
			break;
		case "3":
			System.out.println("Wir befinden uns in Modul 3");
			break;

		}

		Raum zimmer2 = new Raum();
		Raum zimmer3 = new Raum();


		zimmer2.modul.setModulID(2);
		zimmer3.modul.setModulID(3);




	}

}
