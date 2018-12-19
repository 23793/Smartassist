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

	private Raum raum;
	private int tempZielWert;
	private AnchorPane apane;
	private Scene scene;

	// Constructor with room
	public Lichtpop(Raum r) {
		raum = r;
	}

	public void display(Stage primaryStage) {

		try {
			apane = (AnchorPane) FXMLLoader.load(getClass().getResource("Lichtkonfig.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte fxml nicht finden!");
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		ObservableList<Node> obj = apane.getChildren();

		// Create Toggleswitches
		ToggleSwitch ts = new ToggleSwitch();
		ToggleSwitch oo = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0);
		hb.getChildren().add(1, ts);
		HBox hb2 = (HBox) b.getChildren().get(1);
		hb2.getChildren().add(1, oo);
		// Initialize Toggleswitches
		ts.set_mode(raum.getLicht().getLichtModus());
		oo.set_mode(raum.getLicht().getLichtAnAus());

		// Create and initialize Slider
		Slider slider = (Slider) b.getChildren().get(3);
		slider.setValue(raum.getLicht().getLichtZielWert());
		tempZielWert = raum.getLicht().getLichtZielWert();
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		// Slider changelistener
		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.0f", NewValue));
				System.out.println(String.format("%.0f", NewValue));
				tempZielWert = Integer.parseInt((String.format("%.0f", NewValue)));
			}
		});

		// Switch to Manual mode if ON/OFF is switched
		oo.switchOnProperty().addListener(new ChangeListener<Boolean>() {
			@Override
			public void changed(ObservableValue<? extends Boolean> arg0, Boolean arg1, Boolean arg2) {
				ts.set_mode(false);
			}

		});

		b.getChildren().add(4, value);
		Button button = (Button) b.getChildren().get(5);

		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				raum.getLicht().setLichtAnAus(oo.get_mode());
				raum.getLicht().setLichtModus(ts.get_mode());
				raum.getLicht().setLichtZielWert(tempZielWert);
				primaryStage.close();
			}
		});

		scene = new Scene(apane);
		primaryStage.setScene(scene);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.setResizable(false);
		primaryStage.show();
	}

}
