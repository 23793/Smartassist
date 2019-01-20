package GUI.src;

import java.beans.PropertyChangeListener;
import java.beans.PropertyChangeSupport;

/**
 * The ObservableBoolean class contains a boolean flag that can be listened to
 * and an event is created upon a change of the value of the flag.
 * 
 * @author Max
 *
 */
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
