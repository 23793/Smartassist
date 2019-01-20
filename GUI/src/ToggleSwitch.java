package GUI.src;

import javafx.beans.property.SimpleBooleanProperty;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;

//automatisch = true = 1
//manuell = false = 0

/**
 * The ToggleSwitch class is used to create toggle switches in the GUI of the
 * temperature and light pop-ups.
 * 
 * @author Minh
 * 
 * @see Temppop
 * @see Lichtpop
 *
 */
public class ToggleSwitch extends HBox {

	private final Label label = new Label();
	private final Button button = new Button();
	private boolean mode = false;

	private SimpleBooleanProperty switchedOn = new SimpleBooleanProperty(false);

	public SimpleBooleanProperty switchOnProperty() {
		return switchedOn;
	}

	private void init() {

		getChildren().addAll(label, button);
		button.setOnAction((e) -> {
			switchedOn.set(!switchedOn.get());
			mode = !mode;
			// System.out.println(mode); // modus wird automatisch (1) gesetzt
		});
		label.setOnMouseClicked((e) -> {
			switchedOn.set(!switchedOn.get());
			mode = !mode;
			// System.out.println(mode);
		});
		setStyle();
		bindProperties();
	}

	public void set_mode(boolean b) {
		mode = b;
		switchedOn.set(b);
	}

	public boolean get_mode() {
		return mode;
	}

	private void setStyle() {
		// Default Width
		setWidth(80);
		label.setAlignment(Pos.CENTER);
		setStyle("-fx-background-color: grey; -fx-text-fill:black; -fx-background-radius: 4;");
		setAlignment(Pos.CENTER_LEFT);
	}

	private void bindProperties() {
		label.prefWidthProperty().bind(widthProperty().divide(2));
		label.prefHeightProperty().bind(heightProperty());
		button.prefWidthProperty().bind(widthProperty().divide(2));
		button.prefHeightProperty().bind(heightProperty());
	}

	public ToggleSwitch() {
		init();
		switchedOn.addListener((a, b, c) -> {
			if (c) {
				setStyle("-fx-background-color: MEDIUMAQUAMARINE;");
				label.toFront();
			} else {
				setStyle("-fx-background-color: grey;");
				button.toFront();
			}
		});
	}
}
