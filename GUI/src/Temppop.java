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

public class Temppop {

	private Raum raum;
	private float tempZielwert;

	private Scene scene;
	private AnchorPane test;

	// Constructor with room
	public Temppop(Raum r) {
		raum = r;
	}

	public void display(Stage primaryStage) throws Exception {

		try {
			test = (AnchorPane) FXMLLoader.load(getClass().getResource("Temptest.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte fxml nicht finden!");
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		/*
		 * add the AnchorPane into a Scene
		 */
		scene = new Scene(test);
		/*
		 * get the single child of the the root
		 */

		ObservableList<Node> obj = test.getChildren();

		// Create the Toggleswitch
		ToggleSwitch ts = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0); // HBox Objekt
		hb.getChildren().add(1, ts); // in die HBox hinzufügen

		// Initialize the Toggleswitch
		ts.set_mode(raum.getKlima().getHeizungsstatus());
		tempZielwert = (float) raum.getKlima().getZielTemp();

		// Create and initialize slider
		Slider slider = (Slider) b.getChildren().get(2);
		slider.setValue(raum.getKlima().getZielTemp());
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.2f", NewValue.floatValue()));
				tempZielwert = Float.parseFloat(String.format("%s", NewValue));
			}
		});

		b.getChildren().add(3, value);
		Button button = (Button) b.getChildren().get(4);

		// ON CLOSE BUTTON PRESS
		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				raum.getKlima().setHeizungsstatus(ts.get_mode());
				raum.getKlima().setZielTemp(tempZielwert);
				raum.getKlima().setImageAndLabel(raum.getModul().temperaturanzeige((float)raum.getKlima().getZielTemp()));

				// Aktualisierte Daten ans WSN senden
				Gui.updateModule(raum);

				primaryStage.close();
			}
		});
		primaryStage.setScene(scene);
		primaryStage.setResizable(false);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.showAndWait();
	}

}
