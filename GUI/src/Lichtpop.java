package GUI.src;

import java.io.IOException;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
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

public class Lichtpop {

	private int licht_zielwert = 1;
	private boolean licht_automatik;
	AnchorPane apane;
	Scene scene;

	public void display(Stage primaryStage) {

		try {
			apane = (AnchorPane) FXMLLoader.load(getClass().getResource("Lichtkonfig.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte Temptest.fxml nicht finden!");
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		ObservableList<Node> obj = apane.getChildren();
		ToggleSwitch ts = new ToggleSwitch();
		ToggleSwitch oo = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0);
		hb.getChildren().add(1, ts);
		HBox hb2 = (HBox) b.getChildren().get(1);
		hb2.getChildren().add(1,oo);

		Slider slider = (Slider) b.getChildren().get(3);
		slider.setValue(get_licht_zielwert());
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.0f", NewValue)); // Wert wird auf
																// dem
																// Bildschirm
																// ausgegeben
				System.out.println(String.format("%.0f", NewValue)); // in der
																	// Konsole
				set_licht_zielwert(String.format("%.0f", NewValue));
			}
		});

		b.getChildren().add(4, value);
		Button button = (Button) b.getChildren().get(5);

		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				System.out.println("saved.");
				System.out.println("Ziellichtwert: " +get_licht_zielwert());
				set_licht_automatik(ts.get_mode());
				System.out.print("Modus: " +get_licht_automatik());
				primaryStage.close();
			}
		});


		scene = new Scene(apane);
		primaryStage.setScene(scene);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.setResizable(false);
		primaryStage.show();
	}

	public void set_licht_zielwert(String wert) {
		licht_zielwert = Integer.parseInt(wert);
	}

	public int get_licht_zielwert() {
		return licht_zielwert;
	}

	public void set_licht_automatik(boolean a) {
		licht_automatik = a;
	}

	public boolean get_licht_automatik() {
		return licht_automatik;
	}


}
