package GUI.src;

import java.awt.Rectangle;
import java.util.ArrayList;

import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.SplitPane;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.TransferMode;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

/**
 * Die Gui Klasse enth�lt die gesamte FXML basierte GUI inklusive aller
 * Funktionalit�ten. Die Funktion getRaumListe() gibt eine ArrayList mit allen
 * R�umen zur�ck.
 * 
 * @see javafx.application.Application
 * @see Raum
 * 
 * @author MinhMax & Gaitan
 *
 */
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
		 * Setzt die X und Y Werte f�r den Punkt des Klickens der Maus fest.
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
		 * Setzt die X und Y Werte f�r den Punkt des Loslassens der Maus fest.
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

		/*
		 * Drag and Drop feature. Starting the Drag-and-Drop Gesture on every
		 * child of the VBox except the logo.
		 */

		VBox vbox = (VBox) splitpane.getItems().get(0);
		ObservableList<Node> list = FXCollections.observableArrayList();

		list.add(vbox.getChildren().get(1));
		list.add(vbox.getChildren().get(2));
		list.add(vbox.getChildren().get(3));
		list.add(vbox.getChildren().get(4));

		for (Node n : list) {
			n.setOnDragDetected(new EventHandler<MouseEvent>() {
				public void handle(MouseEvent event) {
					Dragboard db = n.startDragAndDrop(TransferMode.MOVE);
					ClipboardContent content = new ClipboardContent();
					content.putString("Hallo");
					db.setContent(content);
					System.out.println("event.getX()  : " + event.getX());
					System.out.println("event.getY()  : " + event.getY());
					System.out.println("event.getTarget() : " + event.getTarget());
					System.out.println("event.getSource() : " + event.getSource());
					// event.ge
					event.consume();
					// event.isConsumed();
				}
			});
		}
		anchorpane.setOnDragOver(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				if (event.getGestureSource() != anchorpane && event.getDragboard().hasString()) {
					/*
					 * allow for both copying and moving, whatever user chooses
					 */
					event.acceptTransferModes(TransferMode.MOVE);
				}

				System.out.println("event.getX()  : " + event.getX());
				System.out.println("event.getY()  : " + event.getY());
				System.out.println("event.getTarget() : " + event.getTarget());
				System.out.println("event.getSource() : " + event.getSource());

				event.consume();
			}
		});

		anchorpane.setOnDragDropped(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				Dragboard db = event.getDragboard();
				boolean success = true;
				// if (db.hasString()) {
				// rightAnchorPane.setAccessibleText(db.getString());
				//// rightAnchorPane.setText(db.getString());
				// success = true;
				// }

				event.setDropCompleted(success);
				System.out.println("event.getX()  : " + event.getX());
				System.out.println("event.getY()  : " + event.getY());
				System.out.println("event.getTarget() : " + event.getTarget());
				System.out.println("event.getSource() : " + event.getSource());
				event.consume();
			}
		});

		vbox.setOnDragDone(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				/* the drag and drop gesture ended */
				/* if the data was successfully moved, clear it */
				if (event.getTransferMode() == TransferMode.MOVE) {
					// vbox.setText("");

				}
				System.out.println("event.getX()  : " + event.getX());
				System.out.println("event.getY()  : " + event.getY());
				System.out.println("event.getTarget() : " + event.getTarget());
				System.out.println("event.getSource() : " + event.getSource());
				event.consume();
			}
		});

		/*
		 * Handling the DRAG_OVER Event on the right AnchorPane.
		 */
		// canvas.setOnDragOver(value);
		anchorpane.setOnDragOver(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				/* data is dragged over the target */
				/*
				 * accept it only if it is not dragged from the same node and if
				 * it has a string data
				 */
				if (event.getGestureSource() != anchorpane && event.getDragboard().hasString()) {
					/*
					 * allow for both copying and moving, whatever user chooses
					 */
					event.acceptTransferModes(TransferMode.MOVE);
				}

				event.consume();
			}
		});

		anchorpane.setOnDragDropped(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				Dragboard db = event.getDragboard();
				boolean success = true;
				// if (db.hasString()) {
				// rightAnchorPane.setAccessibleText(db.getString());
				//// rightAnchorPane.setText(db.getString());
				// success = true;
				// }

				event.setDropCompleted(success);
				System.out.println("event.getX()  : " + event.getX());
				System.out.println("event.getY()  : " + event.getY());
				System.out.println("event.getTarget() : " + event.getTarget());
				System.out.println("event.getSource() : " + event.getSource());
				event.consume();
			}
		});

		// Set the scene to the stage
		primaryStage.setScene(scene);

		// Set the title of the stage
		primaryStage.setTitle("SmartAsssit");

		// Display the stage
		primaryStage.show();

	}

	/**
	 * Zeichnet ein sichtbares Rectangle in das Grundrissfenster und erstellt
	 * ein Objekt vom Typ Raum. Gezeichnete R�ume m�ssen mindestens 50x50 Pixel
	 * gro� sein und d�rfen sich nicht �berschneiden.
	 * 
	 * @see Raum
	 * @param gc
	 */
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
			// Punkt f�r eigentlichen Raum unten rechts
			raumListe.add(new Raum(idCounter, viereck.x + viereck.width, viereck.y + viereck.height, viereck));
			rectangles.add(viereck);
			System.out.println("Raum: " + raumListe.get(idCounter - 1).getID() + ", "
					+ raumListe.get(idCounter - 1).getposition_x() + ", "
					+ raumListe.get(idCounter - 1).getposition_y());
			idCounter++;
		}

		System.out.println("Raumliste:" + raumListe.size());
		System.out.println("Breite: " + viereck.width);
		System.out.println("Hoehe: " + viereck.height);

	}

	public static ArrayList<Raum> getRaumListe() {
		return raumListe;
	}

	public static void setRaumListe(ArrayList<Raum> raumListe) {
		Gui.raumListe = raumListe;
	}
}
