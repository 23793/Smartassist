package GUI.src;

import java.io.IOException;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.event.ActionEvent;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Modality;
import javafx.stage.Stage;

/**
 * Temppop class for the temperature configuration pop-up
 * 
 * @author MinhMax
 *
 */
public class Temppop {

	private Raum raum;
	private float tempZielwert;

	private Scene scene;
	private AnchorPane test;

	// Constructor with room
	public Temppop(Raum r) {
		raum = r;
	}

	/**
	 * Displays the temperature pop-up and handles all logical functionalities
	 * 
	 * @param primaryStage
	 *            the stage the pop-up is called from
	 */
	public void display(Stage primaryStage) throws Exception {

		// Load the FXML
		try {
			test = (AnchorPane) FXMLLoader.load(getClass().getResource("Temptest.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte fxml nicht finden!");
			e.printStackTrace();
		}

		// Add the AnchorPane into a Scene
		scene = new Scene(test);

		ObservableList<Node> obj = test.getChildren();

		// Create the ToggleSwitch
		ToggleSwitch ts = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0); // HBox object
		hb.getChildren().add(1, ts); // add Switch to HBox

		// Initialize the ToggleSwitch
		ts.set_mode(raum.getKlima().getHeizungsstatus());
		tempZielwert = (float) raum.getKlima().getZielTemp();

		// Create and initialize the slider
		Slider slider = (Slider) b.getChildren().get(2);
		slider.setValue(raum.getKlima().getZielTemp());
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		// Change display value as the slider is being dragged around
		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.2f", NewValue.floatValue()));
				tempZielwert = Float.parseFloat(String.format("%s", NewValue));
			}
		});

		// Current value display Label
		b.getChildren().add(3, value);

		// Close button
		Button button = (Button) b.getChildren().get(4);
		// On close button press
		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				// Update the room settings
				raum.getKlima().setHeizungsstatus(ts.get_mode());
				raum.getKlima().setZielTemp(tempZielwert);
				// Update the temperature icon
				raum.getKlima()
						.setImageAndLabel(raum.getModul().temperaturanzeige((float) raum.getKlima().getZielTemp()));

				// Send new settings to the WSN
				Gui.updateModule(raum);

				// Close the pop-up
				primaryStage.close();
			}
		});

		// Pop-up window settings
		primaryStage.setScene(scene);
		primaryStage.setResizable(false);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.showAndWait();
	}

}
