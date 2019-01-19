package GUI.src;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

public class ObservableBoolean {

	protected PropertyChangeSupport propertyChangeSupport;
	private Boolean flag;

	public ObservableBoolean(Boolean initialValue) {
		this.flag = initialValue;
		propertyChangeSupport = new PropertyChangeSupport(this);
	}

	public void setFlag(Boolean flag) {
		Boolean oldFlag = this.flag;
		this.flag = flag;
		propertyChangeSupport.firePropertyChange("BooleanProperty", oldFlag, flag);
	}

	public Boolean getFlag() {
		return flag;
	}

	public void addPropertyChangeListener(PropertyChangeListener listener) {
		propertyChangeSupport.addPropertyChangeListener(listener);
	}

}
