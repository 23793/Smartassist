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

	private float temp_zielwert = 22.00f;
	private boolean temp_automatik;

	Scene scene;
	AnchorPane test;

	public void display(Stage primaryStage) throws Exception {

		try {
			test = (AnchorPane) FXMLLoader.load(getClass().getResource("Temptest.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte Temptest.fxml nicht finden!");
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
		ToggleSwitch ts = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0); // HBox Objekt
		hb.getChildren().add(1, ts); // in die HBox hinzufügen

		Slider slider = (Slider) b.getChildren().get(2);
		slider.setValue(get_temp_zielwert());
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.2f", NewValue)); // Wert wird auf
																// dem
																// Bildschirm
																// ausgegeben
				System.out.println(String.format("%.2f", NewValue)); // in der
																		// Konsole
				set_temp_zielwert(String.format("%s",NewValue));
			}
		});

		b.getChildren().add(3, value);
		Button button = (Button) b.getChildren().get(4);

		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				System.out.println("saved.");
				System.out.println("Zieltemp: " +get_temp_zielwert());
				primaryStage.close();
			}
		});
		primaryStage.setScene(scene);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.showAndWait();
	}

	public void set_temp_zielwert(String string) {
		Float test = Float.parseFloat(string);
		temp_zielwert = Float.parseFloat(string);
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
