package GUI.src;


public class Temppop {

	private float temp_zielwert;
	private boolean temp_automatik;

	public void set_temp_zielwert(float wert) {
		temp_zielwert = wert;
	}

	public float get_temp_zielwert() {
		return temp_zielwert;
	}

	public void set_temp_automatik(boolean a) {
		temp_automatik = a;
	}

	public boolean get_temp_automatik() {
		return temp_automatik;
	}
}
