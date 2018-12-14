package GUI.src;

import java.awt.Point;
import java.awt.Rectangle;
import java.util.ArrayList;

import javafx.application.Application;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.event.ActionEvent;
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

	Image fire = new Image("/GUI/resources/fire.png", true);
	Image temp = new Image("/GUI/resources/temp.png", true);
	Image snow = new Image("/GUI/resources/snow.png", true);
	Image perfect = new Image("/GUI/resources/perfect.png", true);

	ImageView iv1 = new ImageView();
	ImageView iv2 = new ImageView();

	@Override
	public void start(Stage primaryStage) throws Exception {

		/*
		 * Skalierung der Bilder
		 */
		iv2.setFitWidth(50);
		iv2.setFitHeight(50);
		iv1.setFitHeight(21);
		iv1.setFitWidth(21);

		/*
		 * Button als Thermometerbild
		 */
		iv2.setImage(temp);
		Button settings = new Button(null, new ImageView(temp));
		settings.setBackground(null);

		/*
		 * Vbox und Hbox für Temperaturanzeige
		 */
		VBox vebox = new VBox();
		HBox box = new HBox();

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
		 * Setzt die X und Y Werte fï¿½r den Punkt des Klickens der Maus fest.
		 */
		anchorpane.setOnMousePressed(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				pressedX = event.getX();
				pressedY = event.getY();
				// System.out.println("X = " + pressedX);
				// System.out.println("Y = " + pressedY);
			}
		});

		/*
		 * Setzt die X und Y Werte fï¿½r den Punkt des Loslassens der Maus fest.
		 */
		anchorpane.setOnMouseReleased(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				releasedX = event.getX();
				releasedY = event.getY();
				// System.out.println("Release X = " + releasedX);
				// System.out.println("Release Y = " + releasedY);
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
					// System.out.println("event.getX() : " + event.getX());
					// System.out.println("event.getY() : " + event.getY());
					// System.out.println("event.getTarget() : " +
					// event.getTarget());
					// System.out.println("event.getSource() : " +
					// event.getSource());
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
				boolean success = false;

				Point p = new Point();
				p.setLocation(event.getX(), event.getY());

				if (tempRaum.getRect().contains(p)) {
					if (tempModulID == 0) {
						tempRaum.setLicht(new Licht(p));
						System.out.println("Licht hinzugefügt!");
						tempRaum = null;
					} else {
						tempRaum.setModul(new Modul(tempModulID));
						System.out.println("Modul " + tempRaum.getModul().getModulID() + " hinzugefügt!");
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
						tempModulID = 0;
						tempRaum = null;
					}
				}
				event.setDropCompleted(success);
				event.consume();
			}
		});
		
		/*
		 * Position der Temperaturanzeige HIER: ersetzten mit X und Y Werten
		 * eines Raumes.
		 */
		vebox.setLayoutX(200);
		vebox.setLayoutY(200);

		/*
		 * TESTMODUL ERSTELLT
		 */
		Modul modul = new Modul(0);
		modul.settemperatur(22.00f);
		modul.tempsettings.set_temp_zielwert(22.00f);
		Label temps = new Label(); // temperatur in Grad anzeigen
		temps.setText(String.format("%.2f", modul.gettemperatur()) + "°C");

		/*
		 * Methode aufrufen und aktuelle Temperatur übergeben und als String
		 * speichern
		 */
		String test = modul.temperaturanzeige(modul.gettemperatur());

		/**
		 * checkt den returnwert von temperaturanzeige() und gibt entsprechendes
		 * Bild aus.
		 */
		if (test.equals("kalt")) {
			iv1.setImage(snow);
		} else if (test.equals("heiss")) {
			iv1.setImage(fire);
		} else if (test.equals("perfekt")) {
			iv1.setImage(perfect);
		}

		settings.setOnAction(new EventHandler<ActionEvent>() {

			@Override
			public void handle(ActionEvent e) {
				// TODO Auto-generated method stub
				System.out.println("Test");
			}

		});

		/*
		 * Falls Modulstatus = aktiv, Temperaturanzeige hinzufügen
		 */
		// If(Modulstatus==1) {
		anchorpane.getChildren().add(vebox);
		vebox.getChildren().add(settings); // Temperaturicon
		vebox.getChildren().add(box);
		box.getChildren().add(temps); // Aktuelle Temperatur
		box.getChildren().add(iv1); // currenticon

		// }

		// Set the scene to the stage
		primaryStage.setScene(scene);

		// Set the title of the stage
		primaryStage.setTitle("SmartAssist");

		// Display the stage
		primaryStage.show();

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

		gc.setLineDashes(5);
		gc.setStroke(Color.WHITE);

		// Check if drawn room is viable
		Boolean intersect = false;
		if (viereck.width < 50 || viereck.height < 50 || (releasedX > 554 || releasedX < 0)
				|| (releasedY > 746 || releasedY < 0)) {
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
