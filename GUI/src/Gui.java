package GUI.src;

import java.awt.Rectangle;
import java.util.ArrayList;

import javafx.application.Application;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.SplitPane;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class Gui extends Application {

	private double pressedX;
	private double pressedY;
	private double releasedX;
	private double releasedY;
	private static ArrayList<Rectangle> rectangles = new ArrayList<Rectangle>();
	private static ArrayList<Raum> raumListe = new ArrayList<Raum>();
	private static int idCounter = 1;

	@Override
	public void start(Stage primaryStage) throws Exception {

		/*
		 * create the AnchorPane and all details and load the Path of the FXML
		 * File
		 */
		AnchorPane root = (AnchorPane) FXMLLoader.load(getClass().getResource("RoomView.fxml"));
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
		 * get the right child of the splitpane and connect a Graphicscontext to
		 * the Canvas and finally set the color of the stroke to red.
		 */
		AnchorPane anchorpane = (AnchorPane) splitpane.getItems().get(1);
		Canvas canvas = (Canvas) anchorpane.getChildren().get(1);
		GraphicsContext gc = canvas.getGraphicsContext2D();
		// Button licht = (Button) anchorpane.getChildren().get(2);
		// licht.setOnAction(event -> GUI.src.Lichtkonfig.display());
		// Button temperature = (Button) anchorpane.getChildren().get(3);
		// temperature.setOnAction(event -> GUI.src.Tempkonfig.display());
		/*
		 * First set the mouse event on the right side of the splitpane and get
		 * the X and Y on mouse pressed and print these out.
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
		 * secondly set the mouse event on the right side of the splitpane and
		 * get the X and Y on mouse released and print these out.
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

		// Set the scene to the stage
		primaryStage.setScene(scene);

		// Set the title of the stage
		primaryStage.setTitle("SmartAsssit");

		// Display the stage
		primaryStage.show();

	}

	private void drawRectangle(GraphicsContext gc) {

		// Hoehe und Breite berechnen
		Rectangle viereck = new Rectangle();

		if (releasedX > pressedX && releasedY > pressedY) {
			// Oben links nach unten rechts
			viereck.width = (int) (releasedX - pressedX);
			viereck.height = (int) (releasedY - pressedY);
			viereck.x = (int) (pressedX);
			viereck.y = (int) (pressedY);
		} else if (pressedX > releasedX && pressedY > releasedY) {
			// Unten rechts nach oben links
			viereck.width = (int) (pressedX - releasedX);
			viereck.height = (int) (pressedY - releasedY);
			viereck.x = (int) (pressedX - viereck.width);
			viereck.y = (int) (releasedY);
		} else if (pressedX > releasedX && pressedY < releasedY) {
			// Oben rechts nach unten links
			viereck.width = (int) (pressedX - releasedX);
			viereck.height = (int) (releasedY - pressedY);
			viereck.x = (int) (releasedX);
			viereck.y = (int) (releasedY - viereck.height);
		} else {
			// Unten links nach oben rechts
			viereck.width = (int) (releasedX - pressedX);
			viereck.height = (int) (pressedY - releasedY);
			viereck.x = (int) (releasedX - viereck.width);
			viereck.y = (int) (pressedY - viereck.height);
		}

		gc.setLineDashes(5);
		gc.setStroke(Color.WHITE);

		Boolean intersect = false;
		for (Rectangle r : rectangles) {
			if (r.intersects(viereck) || viereck.width < 50 || viereck.height < 50) {
				intersect = true;
				System.out.println("Rooms cannot intersect!");
				break;
			}
		}
		if (!intersect) {
			gc.strokeRect(viereck.x, viereck.y, viereck.width, viereck.height);
			// Punkt für eigentlichen Raum unten rechts
			raumListe.add(new Raum(idCounter, viereck.x + viereck.width, viereck.y + viereck.height));
			rectangles.add(viereck);
			System.out.println("Raum: " + raumListe.get(idCounter - 1).getID() + ", "
					+ raumListe.get(idCounter - 1).getposition_x() + ", "
					+ raumListe.get(idCounter - 1).getposition_y());
			idCounter++;
		}

		System.out.println("Raumliste:" + raumListe.size());
		System.out.println("Breite: " + viereck.width);
		System.out.println("HÃ¶he: " + viereck.height);

	}

	public static ArrayList<Raum> getRaumListe() {
		return raumListe;
	}

	public static void setRaumListe(ArrayList<Raum> raumListe) {
		Gui.raumListe = raumListe;
	}
}
