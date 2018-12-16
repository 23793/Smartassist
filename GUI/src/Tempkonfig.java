package GUI.src;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Slider;
import javafx.scene.layout.AnchorPane;
//import javafx.scene.layout.GridPane;
//import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Modality;
//import javafx.scene.paint.Color;
//import javafx.stage.Modality;
import javafx.stage.Stage;

public class Tempkonfig implements EventHandler<ActionEvent> {

	Button save = new Button("OK"); //Speichern button
	String temperatur = ""; //String temp
	ToggleSwitch toggleswitch = new ToggleSwitch(); //onoffbutton
	Button temperature = new Button("close");


	public void display(Stage primaryStage) throws Exception {
	AnchorPane test = (AnchorPane) FXMLLoader.load(getClass().getResource("Temptest.fxml"));
//	Scene scene = new Scene(test);
	ObservableList<Node> obj = test.getChildren();

		primaryStage.initModality(Modality.APPLICATION_MODAL);
		primaryStage.setTitle("Temperaturkonfiguration"); //Titel
		VBox root = new VBox(); //vertikale box
		root.setAlignment(Pos.CENTER); //Position der Objekte
		root.setStyle("dimgrey"); // change the color to dimgrey
		Label manuell = new Label("Manuell: ");
		root.getChildren().add(manuell); //Label mit text einbetten

		toggleswitch.setMaxWidth(100); //breite des on off buttons
		root.getChildren().add(toggleswitch); //einbetten

		Label temp = new Label ("Temperatureinstellung: ");
		root.getChildren().add(temp);

		Slider slider = new Slider(0, 1, 0.5); //slider
		root.getChildren().add(slider);
		slider.setMax(40.0);	//h�chster wert 40
		slider.setMaxWidth(300);	//breite des sliders
		slider.setShowTickLabels(true); //Zahlen werden angezeigt
		slider.setShowTickMarks(true); //einzelne Striche (marks)
		slider.setMajorTickUnit(10.f); //Zahlen werden in 10er Schritten angezeigt
		slider.setSnapToTicks(true); //0.25 Schritte

		root.setPadding(new Insets(40));
		Label value = new Label(Double.toString(slider.getValue()));

		slider.valueProperty().addListener(new ChangeListener<Number>() {

			public void changed(ObservableValue<? extends Number> observable, Number oldValue, Number NewValue) {

				value.setText(String.format("%.2f", NewValue)); //Wert wird auf dem Bildschirm ausgegeben
				System.out.println(String.format("%.2f", NewValue)); //in der Konsole
				temperatur = String.format("%.2f", NewValue);	//wert wird in temp eingespeichert
			}
		});

		root.getChildren().add(value);
		root.getChildren().add(save);
		save.setOnAction(this);
		Label lb = new Label("Temperatur Fenster schließen:");
		root.getChildren().add(lb); // add the label lb into the vbox
		temperature.setOnAction(event -> primaryStage.close()); // close the popUp window
		root.getChildren().add(temperature);
		Scene scene = new Scene(test);  //gr��e der anwendung
		primaryStage.setScene(scene);
		primaryStage.showAndWait();

	}

	public void handle(ActionEvent event) {

		if(event.getSource()==save) {
			System.out.println("SAVED: ");
			System.out.println("Zieltemp :" +temperatur);
			System.out.println("Modus (0=manuell) (1=automatisch) : " +toggleswitch.get_modus());
		}
	}

	public void init() throws Exception {
		System.out.println("bin in der Init");
	}

}

