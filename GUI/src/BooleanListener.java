package GUI.src;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

/**
 * The BooleanListener class containing the propertyChange() event handler.
 * 
 * @author Max
 *
 */
public class BooleanListener implements PropertyChangeListener {
	@Override
	public void propertyChange(PropertyChangeEvent event) {
		if (event.getPropertyName().equals("BooleanProperty")) {
			System.out.println("Lightswitch");
		}
	}
}
