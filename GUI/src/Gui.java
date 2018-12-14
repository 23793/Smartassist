package GUI.src;

import java.awt.Point;
import java.awt.Rectangle;
import java.util.ArrayList;

import application.Modul;
import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.SplitPane;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.TransferMode;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.HBox;
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
	private static int tempModulID = 0;
	private static ObservableList<Node> list = FXCollections.observableArrayList();

	@Override
	public void start(Stage primaryStage) throws Exception {

		Image fire = new Image("/fire.png", true);
		Image perfect = new Image("/perfect.png", true);
		Image snow = new Image("/snow.png", true);
		Image temp = new Image("/temp.png", true);

		ImageView iv1 = new ImageView();
		ImageView iv2 = new ImageView();
		iv2.setFitWidth(50);
		iv2.setFitHeight(50);
		iv1.setFitHeight(21);
		iv1.setFitWidth(21);
		iv2.setImage(temp);

		Button settings = new Button(null, new ImageView(temp));


		/*
		 * create the AnchorPane and all details and load the Path of the FXML
		 * File
		 */
		AnchorPane root = (AnchorPane) FXMLLoader.load(getClass().getResource("RoomView.fxml"));
		VBox vebox = new VBox();
		HBox box = new HBox();
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
		root.getChildren().add(vebox);
		vebox.getChildren().add(settings);		//Temperaturicon
		vebox.getChildren().add(box);
		Label temps = new Label();
		settings.setBackground(null);
		Modul modul = new Modul(0);

		modul.settemperatur(25.00f);
		modul.tempsettings.set_temp_zielwert(22.00f);
		temps.setText(String.format("%.2f", modul.gettemperatur()));

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
					if (n.getId() == list.get(0).getId()) {
						tempModulID = 1;
					} else if (n.getId() == list.get(1).getId()) {
						tempModulID = 2;
					} else if (n.getId() == list.get(2).getId()) {
						tempModulID = 3;
					} else if (n.getId() == list.get(3).getId()) {
						tempModulID = 0;
					}
					event.consume();

				}
			});
		}
		anchorpane.setOnDragOver(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				if (event.getGestureSource() != anchorpane && event.getDragboard().hasString()) {

					event.acceptTransferModes(TransferMode.MOVE);
				}
				event.consume();
			}
		});

		anchorpane.setOnDragDropped(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				// Dragboard db = event.getDragboard();
				boolean success = true;
				Point p = new Point();
				p.setLocation(event.getX(), event.getY());
				int index = 0;
				for (Raum r : raumListe) {
					if (r.getRect().contains(p) && r.getModul() == null) {
						if (tempModulID == 0) {
							r.setLicht(new Licht());
						} else {
							r.setModul(new Modul(tempModulID));
							System.out.println(" Modul : " + r.getModul().getModulID());
							tempModulID = 0;
							break;
						}
					}
					index++;
				}
				event.setDropCompleted(success);
				event.consume();
			}
		});

		// Set the scene to the stage
		primaryStage.setScene(scene);

		// Set the title of the stage
		primaryStage.setTitle("SmartAssist");

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
