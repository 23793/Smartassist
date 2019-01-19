package GUI.src;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

public class BooleanListener implements PropertyChangeListener {
	@Override
	public void propertyChange(PropertyChangeEvent event) {
		if (event.getPropertyName().equals("BooleanProperty")) {
			System.out.println("Lightswitch");
		}
	}
}
