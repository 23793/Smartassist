package GUI.src;

import java.beans.PropertyChangeEvent;
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
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Modality;
import javafx.stage.Stage;

/**
 * Lichtpop class for the light configuration pop-up
 * 
 * @author MinhMax
 *
 */
public class Lichtpop {

	private Raum raum;
	private int tempZielWert;
	private AnchorPane apane;
	private Scene scene;

	// Constructor with room
	public Lichtpop(Raum r) {
		raum = r;
	}

	// Icons for low / high light
	private Image moon = new Image("/GUI/resources/moon.png", true);
	private Image sun = new Image("/GUI/resources/sun.png", true);

	private ImageView iv1 = new ImageView();
	private ImageView iv2 = new ImageView();

	/**
	 * Displays the light pop-up and handles all logical functionalities
	 * 
	 * @param primaryStage
	 *            the stage the pop-up is called from
	 */
	public void display(Stage primaryStage) {

		iv1.setImage(moon);
		iv1.setX(5);
		iv1.setY(30);
		iv2.setImage(sun);

		// Loads the FXML for the pop-up
		try {
			apane = (AnchorPane) FXMLLoader.load(getClass().getResource("Lichtkonfig.fxml"));
		} catch (IOException e) {
			System.out.println("Konnte fxml nicht finden!");
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		ObservableList<Node> obj = apane.getChildren();

		// Create ToggleSwitches
		ToggleSwitch ts = new ToggleSwitch();
		ToggleSwitch oo = new ToggleSwitch();
		VBox b = (VBox) obj.get(0);
		HBox hb = (HBox) b.getChildren().get(0);
		hb.getChildren().add(1, ts);
		HBox hb2 = (HBox) b.getChildren().get(1);
		hb2.getChildren().add(1, oo);

		// Initialize ToggleSwitches
		ts.set_mode(raum.getLicht().getLichtModus());
		oo.set_mode(raum.getLicht().getLichtAnAus());

		// HBoxes for icons
		HBox z = new HBox();
		apane.getChildren().add(z);
		z.getChildren().add(iv1);
		z.setLayoutX(30);
		z.setLayoutY(165);

		HBox y = new HBox();
		apane.getChildren().add(y);
		y.getChildren().add(iv2);
		y.setLayoutX(380);
		y.setLayoutY(165);

		// Create and initialize Slider
		Slider slider = (Slider) b.getChildren().get(3);
		slider.setValue(raum.getLicht().getLichtZielWert());
		tempZielWert = raum.getLicht().getLichtZielWert();
		Label value = new Label(Double.toString(slider.getValue()));
		value.setTextFill(Color.ANTIQUEWHITE);

		// Slider ChangeListener
		slider.valueProperty().addListener(new ChangeListener<Number>() {
			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.0f", NewValue));
				tempZielWert = Integer.parseInt((String.format("%.0f", NewValue)));
			}
		});

		// Switch to Manual mode if ON/OFF is switched manually
		oo.switchOnProperty().addListener(new ChangeListener<Boolean>() {
			@Override
			public void changed(ObservableValue<? extends Boolean> arg0, Boolean arg1, Boolean arg2) {
				ts.set_mode(false);
			}

		});

		// Close button
		Button button = (Button) b.getChildren().get(5);

		// On close button press
		button.setOnAction(new EventHandler<ActionEvent>() {

			public void handle(ActionEvent event) {
				// Set the new settings for the light
				raum.getLicht().setLichtAnAus(oo.get_mode());
				raum.getLicht().setLichtModus(ts.get_mode());
				raum.getLicht().setLichtZielWert(tempZielWert);

				// Send new settings to the WSN
				Gui.updateModule(raum);

				// Close the pop-up
				primaryStage.close();
			}
		});

		// Pop-up window settings
		scene = new Scene(apane);
		primaryStage.setScene(scene);
		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.setResizable(false);
		primaryStage.show();
	}
}
