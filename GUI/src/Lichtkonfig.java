package GUI.src;

import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
//import javafx.scene.layout.GridPane;
//import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class Lichtkonfig extends Application implements EventHandler<ActionEvent> {

	Button save = new Button("OK"); //Speichern button
	String temperatur = ""; //String temp
	ToggleSwitch toggleswitch = new ToggleSwitch(); //onoffbutton
	ToggleSwitch button = new ToggleSwitch();
//	Button close = new Button("Close");

	public void start(Stage primaryStage) throws Exception {
//		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.setTitle("Lichtkonfiguration"); //Titel
		VBox root = new VBox(); //vertikale box
		root.setAlignment(Pos.CENTER); //Position der Objekte

		Label manuell = new Label("Manuell: ");
		root.getChildren().add(manuell); //Label mit text einbetten

		toggleswitch.setMaxWidth(100); //breite des on off buttons
		root.getChildren().add(toggleswitch); //einbetten

		Label lichtschalter = new Label("Licht: ");
		root.getChildren().add(lichtschalter); //Label mit text einbetten

		button.setMaxWidth(100);
		root.getChildren().add(button);

		Label licht = new Label ("Lichtkonfig: ");
		root.getChildren().add(licht);

		Slider slider = new Slider(0, 0, 1); //slider
		root.getChildren().add(slider);
		slider.setMax(3);	//h�chster wert 40
		slider.setMaxWidth(300);	//breite des sliders
		slider.setShowTickLabels(true); //Zahlen werden angezeigt
		slider.setShowTickMarks(true); //einzelne Striche (marks)
		slider.setMajorTickUnit(1); //Zahlen werden in 1er Schritten angezeigt
		slider.setSnapToTicks(true); //0.25 Schritte

		root.setPadding(new Insets(40));
		Label value = new Label(Double.toString(slider.getValue()));

		slider.valueProperty().addListener(new ChangeListener<Number>() {

			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.0f", NewValue)); //Anzeige im rootfenster
				System.out.println(String.format("%.0f", NewValue));
				temperatur = String.format("%.0f", NewValue);
			}
		});

		root.getChildren().add(value);
		root.getChildren().add(save);
		save.setOnAction(this);
//		Label lb = new Label("Licht schließen:");
//		root.getChildren().add(lb);
//		close.setOnAction(event -> primaryStage.close());
//		root.getChildren().add(close);

		Scene scene = new Scene(root,500,400);  //gr��e der anwendung
		primaryStage.setScene(scene);
//		primaryStage.showAndWait();
		primaryStage.show();
	}


	public void handle(ActionEvent event) {

		if(event.getSource()==save) {
			System.out.println("SAVED: ");
			System.out.println("Zieltemp :" +temperatur);
			System.out.println("Modus (0=manuell) (1=automatisch) : " +toggleswitch.get_modus());
		}
	}

	public void init() throws Exception { //wenn wir objekte speichern, hiermit initialisieren
		System.out.println("bin in der Init");
	}

	public static void main(String[] args) {
		launch(args); //w�hrend die anwendung l�uft
	}
}


