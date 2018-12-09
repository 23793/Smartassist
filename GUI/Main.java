package GUI;

import java.awt.Rectangle;
import java.util.ArrayList;

import GUI.src.Lichtkonfig;
import javafx.scene.Node;

import javafx.application.Application;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Button;
import javafx.scene.control.SplitPane;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import smartassist.model.Popup;

public class Main extends Application{
	
	double pressedX;
	double pressedY;
	double releasedX;
	double releasedY;
	static ArrayList<Rectangle> raumliste = new ArrayList<Rectangle>();
	
	public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {

        /*
         * create the AnchorPane and all details and
         * load the Path of the FXML File
         * */
        AnchorPane root = (AnchorPane)FXMLLoader.load(getClass().getResource("RoomView.fxml"));
        /*
         * add the AnchorPane into a Scene
         */
        Scene scene = new Scene(root);
        /*
         * get the single child of the the root
         */
        ObservableList<Node> obs = root.getChildren();
        SplitPane splitpane = (SplitPane) obs.get(0);
        
        /*
         * get the right child of the splitpane and
         * connect a Graphicscontext to the Canvas and finally
         * set the color of the stroke to red.
         */
        AnchorPane anchorpane = (AnchorPane) splitpane.getItems().get(1);
        Canvas canvas = (Canvas) anchorpane.getChildren().get(1);
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.setStroke(Color.RED);
//        Button licht =  (Button) anchorpane.getChildren().get(2);
//        licht.setOnAction(event -> GUI.src.Lichtkonfig.display());
//        Button temperature = (Button) anchorpane.getChildren().get(3);
//        temperature.setOnAction(event -> GUI.src.Tempkonfig.display());
        /*
         * First set the mouse event on the right side of the splitpane
         * and get the X and Y on mouse pressed and print these out.
         */
 
		anchorpane.setOnMousePressed(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				pressedX = event.getX();
				pressedY = event.getY();
				System.out.println("X = " + pressedX);
				System.out.println("Y = " + pressedY);
			}
		});

        /*
         * secondly set the mouse event on the right side of the splitpane
         * and get the X and Y on mouse released and print these out.
         */
		anchorpane.setOnMouseReleased(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				releasedX = event.getX();
				releasedY = event.getY();
				System.out.println("Release X = " + releasedX);
				System.out.println("Release Y = " + releasedY);
				drawRectangle(gc);
			}
		});

        //Set the scene to the stage
        primaryStage.setScene(scene);

        // Set the title of the stage
        primaryStage.setTitle("SmartAsssit");

        //Display the stage
        primaryStage.show();

    }
    
    private void drawRectangle(GraphicsContext gc) {

		// H�he und Breite berechnen
		double breite = releasedX - pressedX;
		double hoehe = releasedY - pressedY;

	//	 gc.setLineDashes(0); //Breite der Linie
		 gc.setStroke(Color.RED);

		Rectangle viereck = new Rectangle();
		viereck.x = (int) pressedX;
		viereck.y = (int) pressedY;
		viereck.width = (int) breite;
		viereck.height = (int) hoehe;


		Boolean intersect = false;
		for (Rectangle r : raumliste) {
			if (r.intersects(viereck)) {
				intersect = true;
				break;
			}
		}
		if (!intersect) {
			gc.strokeRect(pressedX, pressedY, breite, hoehe);
			raumliste.add(viereck);

		}

		System.out.println("Raumliste:" + raumliste.size());
		System.out.println("Breite: " + breite);
		System.out.println("Höhe: " + hoehe);

	}
		
}