package GUI.src;


public class Klima {

	private boolean heizungsstatus;
	private boolean ACstatus;

	public void set_heizungsstatus(boolean status) {
		heizungsstatus = status;
	}

	public boolean get_heizungsstatus() {
		return heizungsstatus;
	}

	public void set_ACstatus(boolean status) {
		ACstatus = status;
	}

	public boolean get_ACstatus() {
		return ACstatus;
	}
}
