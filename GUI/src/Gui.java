package GUI.src;

import java.awt.Point;
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
import javafx.scene.control.Button;
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
 * Die Gui Klasse enthï¿½lt die gesamte FXML basierte GUI inklusive aller
 * Funktionalitï¿½ten. Die Funktion getRaumListe() gibt eine ArrayList mit allen
 * Rï¿½umen zurï¿½ck.
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
	private static Raum tempRaum = null;
	private static ObservableList<Node> list = FXCollections.observableArrayList();

	private static AnchorPane root;
	private static Scene scene;
	private static ObservableList<Node> obs;
	private static SplitPane splitpane;
	private static AnchorPane anchorpane;
	private static Canvas canvas;
	private static GraphicsContext gc;

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
		scene = new Scene(root);

		/*
		 * get the single child of the the root
		 */
		ObservableList<Node> obs = root.getChildren();
		splitpane = (SplitPane) obs.get(0);

		/*
		 * get the right child of the splitpane and connect a Graphicscontext to
		 * the Canvas and finally set the color of the stroke to red.
		 */
		anchorpane = (AnchorPane) splitpane.getItems().get(1);
		canvas = (Canvas) anchorpane.getChildren().get(1);
		gc = canvas.getGraphicsContext2D();

		/*
		 * Setzt die X und Y Werte fï¿½r den Punkt des Klickens der Maus fest.
		 */
		anchorpane.setOnMousePressed(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				pressedX = event.getX();
				pressedY = event.getY();
			}
		});

		/*
		 * Setzt die X und Y Werte fï¿½r den Punkt des Loslassens der Maus fest.
		 */
		anchorpane.setOnMouseReleased(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				releasedX = event.getX();
				releasedY = event.getY();
				drawRectangle(gc);
			}
		});

		/*
		 * Drag and Drop feature. Starting the Drag-and-Drop Gesture on every
		 * child of the VBox except the logo.
		 */
		// Hardcoded list of all the dragable items on the left pane
		VBox vbox = (VBox) splitpane.getItems().get(0);
		list.add(vbox.getChildren().get(1));
		list.add(vbox.getChildren().get(2));
		list.add(vbox.getChildren().get(3));
		list.add(vbox.getChildren().get(4));

		// Loop for determining which module or if a light is being dragged
		for (Node n : list) {
			n.setOnDragDetected(new EventHandler<MouseEvent>() {
				public void handle(MouseEvent event) {
					Dragboard db = n.startDragAndDrop(TransferMode.MOVE);
					ClipboardContent content = new ClipboardContent();
					content.putString("Hallo");
					db.setContent(content);
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

		// Handling illegal drop positions (\)
		anchorpane.setOnDragOver(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				Point p = new Point();
				p.setLocation(event.getX(), event.getY());
				for (Raum r : raumListe) {
					if (tempModulID != 0) {
						if (r.getRect().contains(p) && r.getModul() == null) {
							tempRaum = r;
							event.acceptTransferModes(TransferMode.MOVE);
						}
					} else {
						if (r.getRect().contains(p) && r.getModul() != null && r.getLicht() == null) {
							tempRaum = r;
							event.acceptTransferModes(TransferMode.MOVE);
						}
					}
				}
				event.consume();
			}
		});

		// Handling the drop and adding new objects to the room aswell as making
		// already used modules unavailable
		anchorpane.setOnDragDropped(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				Point p = new Point();
				p.setLocation(event.getX(), event.getY());

				if (tempRaum.getRect().contains(p)) {
					if (tempModulID == 0) {
						// FÜGT EIN LICHT HINZU
						tempRaum.setLicht(new Licht(p, tempRaum, anchorpane));
						createLichtAnzeige(tempRaum);
						System.out.println("Licht zu Raum " + tempRaum.getID() + " hinzugefügt!");
						tempModulID = 0;
						tempRaum = null;
					} else {
						// ERSTELLT DEN RAUM
						tempRaum.setModul(new Modul(tempModulID));
						System.out.println("Modul " + tempRaum.getModul().getModulID() + " hinzugefügt!");
						// FÜGT TEMPERATURANZEIGE HINZU
						createTempAnzeige(tempRaum);
						// MALE RAUM AUS
						gc.setFill(Color.WHITE);
						gc.fillRect(tempRaum.getRect().getX() + 1, tempRaum.getRect().getY() + 1,
								tempRaum.getRect().getWidth() - 2, tempRaum.getRect().getHeight() - 2);
						// DEAKTIVIERE BEREITS BENUTZTE MODULE
						switch (tempModulID) {
						case 1:
							list.get(0).setOpacity(0.2);
							list.get(0).setDisable(true);
							break;
						case 2:
							list.get(1).setOpacity(0.2);
							list.get(1).setDisable(true);
							break;
						case 3:
							list.get(2).setOpacity(0.2);
							list.get(2).setDisable(true);
							break;
						}
						// RESET TEMP VARIABLES
						tempModulID = 0;
						tempRaum = null;
					}
				}
				event.setDropCompleted(true);
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

	/*
	 * TEMPERATURANZEIGE IM RAUM
	 */
	private void createTempAnzeige(Raum r) {
		// Falls Raum ein Modul hat (nur um sicher zu gehen)
		if (r.getModul() != null) {

			String temperatur = r.getModul().temperaturanzeige(22.00f);
			r.getKlima().setImageAndLabel(temperatur, String.format("%.2f", r.getModul().gettemperatur()));

			anchorpane.getChildren().add(r.getKlima().getVebox());
			// TemperaturIcon
			r.getKlima().getVebox().getChildren().add(r.getKlima().getSettings());
			r.getKlima().getVebox().getChildren().add(r.getKlima().getBox());
			// Aktuelle Temperatur
			r.getKlima().getBox().getChildren().add(r.getKlima().getTemps());
			// Aktuelles Icon
			r.getKlima().getBox().getChildren().add(r.getKlima().getIv1());
		}
	}

	private void createLichtAnzeige(Raum r) {
		anchorpane.getChildren().add(r.getLicht().getVebox());
		// LichtIcon
		r.getLicht().getVebox().getChildren().add(r.getLicht().getSettings());
		r.getLicht().getVebox().getChildren().add(r.getLicht().getBox());
	}

	/**
	 * Zeichnet ein sichtbares Rectangle in das Grundrissfenster und erstellt
	 * ein Objekt vom Typ Raum. Gezeichnete Rï¿½ume mï¿½ssen mindestens 50x50
	 * Pixel groï¿½ sein und dï¿½rfen sich nicht ï¿½berschneiden.
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

		gc.setLineDashes(8);
		gc.setLineWidth(3);
		gc.setStroke(Color.LIGHTGREY);

		// Check if drawn room is viable (big enough, not intersecting, in
		// bounds)
		Boolean intersect = false;
		if (viereck.width < 58 || viereck.height < 72) {
			intersect = true;
			System.out.println("Room too small!");
		}
		if ((releasedX > 554 || releasedX < 0) || (releasedY > 746 || releasedY < 0)) {
			intersect = true;
			System.out.println("Rooms cannot be out of bounds!");
		}
		for (Rectangle r : rectangles) {
			if (r.intersects(viereck)) {
				intersect = true;
				System.out.println("Rooms cannot intersect!");
				break;
			}
		}

		// Draw room if viable
		if (!intersect) {
			gc.strokeRect(viereck.x, viereck.y, viereck.width, viereck.height);
			// Punkt fï¿½r eigentlichen Raum unten rechts
			raumListe.add(new Raum(idCounter, viereck.x + viereck.width, viereck.y + viereck.height, viereck));
			rectangles.add(viereck);
			System.out.println("Raum: " + raumListe.get(idCounter - 1).getID() + ", "
					+ raumListe.get(idCounter - 1).getposition_x() + ", "
					+ raumListe.get(idCounter - 1).getposition_y());
			idCounter++;
		}

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
