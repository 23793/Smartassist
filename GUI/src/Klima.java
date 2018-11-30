package GUI.src;


public class Klima {

	private boolean heizungsstatus;
	private boolean ACstatus;

	public void set_heizungsstatus(boolean s1) {
		heizungsstatus = s1;
	}

	public boolean get_heizungsstatus() {
		return heizungsstatus;
	}

	public void set_ACstatus(boolean s2) {
		ACstatus = s2;
	}

	public boolean get_ACstatus() {
		return ACstatus;
	}
}
