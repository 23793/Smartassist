package GUI.src;

import java.awt.Point;
import java.awt.Rectangle;
import java.text.DecimalFormat;
import java.util.ArrayList;

import javafx.application.Application;
import javafx.application.Platform;
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
import javafx.scene.image.Image;
import javafx.scene.input.ClipboardContent;
import javafx.scene.input.DragEvent;
import javafx.scene.input.Dragboard;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.TransferMode;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;
import javafx.stage.WindowEvent;

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

	// Schnittstelle
	private static Schnittstelle schnittstelle;
	private static String raumString;

	// Variables
	private double pressedX;
	private double pressedY;
	private double releasedX;
	private double releasedY;
	private static ArrayList<Rectangle> rectangles = new ArrayList<Rectangle>();
	private static ArrayList<Raum> raumListe = new ArrayList<Raum>(3);
	private static int idCounter = 1;
	private static int tempModulID = 0;
	private static Raum tempRaum = null;
	private static ObservableList<Node> list = FXCollections.observableArrayList();

	private static Scene scene;
	private static SplitPane splitpane;
	private static AnchorPane anchorpane;
	private static Canvas canvas, canvas2;
	private static GraphicsContext gc, gc2;
	private static Button reset;
	String temperatur = "";
	Stage stage = new Stage();

	@Override
	public void start(Stage primaryStage) throws Exception {
		/*
		 * Kommunikation mit Schnittstelle
		 */
		schnittstelle = new Schnittstelle();

		new Thread(new Runnable() {
			public void run() {
				// Verbindung herstellen
				while (Schnittstelle.getSerialPort() == null) {
					schnittstelle.connect();
					try {
						Thread.sleep(1000);
					} catch (InterruptedException ie) {
						ie.printStackTrace();
					}
				}

				// Loop für datenempfang
				while (Schnittstelle.getSerialPort().isOpened()) {
					
					// Daten Empfangen
					raumString = schnittstelle.receive();
					// Raumdaten aktualisieren
					updateRoom(raumString);

					// Pause bis zum nächsten Receive
					try {
						Thread.sleep(500);
					} catch (InterruptedException ie) {
						ie.printStackTrace();
					}
				}
				
			}
		}).start();

		// Eventhandler zum Beenden des Threads beim Schließen der GUI
		primaryStage.setOnCloseRequest(new EventHandler<WindowEvent>() {
			@Override
			public void handle(WindowEvent t) {
				schnittstelle.close();
				Platform.exit();
				System.exit(0);
			}
		});

		/*
		 * create the AnchorPane and all details and load the Path of the FXML
		 * File
		 */
		AnchorPane root = (AnchorPane) FXMLLoader.load(getClass().getResource("RoomView.fxml"));

		/*
		 * add the AnchorPane into a Scene
		 */

		scene = new Scene(root);

		ObservableList<Node> obs = root.getChildren();
		splitpane = (SplitPane) obs.get(0);

		/*
		 * get the right child of the splitpane and connect a Graphicscontext to
		 * the Canvas and finally set the color of the stroke to red.
		 */
		anchorpane = (AnchorPane) splitpane.getItems().get(1);
		canvas = (Canvas) anchorpane.getChildren().get(1);
		canvas2 = new Canvas(554, 746);
		anchorpane.getChildren().add(canvas2);

		gc = canvas.getGraphicsContext2D();
		gc2 = canvas2.getGraphicsContext2D();

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
				gc2.clearRect(0, 0, 554, 746);
				releasedX = event.getX();
				releasedY = event.getY();
				drawRectangle(gc);
			}
		});

		// PREVIEW Fï¿½R DAS GRUNDRISSZEICHNEN
		anchorpane.setOnMouseDragged(new EventHandler<MouseEvent>() {
			public void handle(MouseEvent event) {
				gc2.clearRect(0, 0, 554, 746);
				Point p = new Point();
				p.setLocation(event.getX(), event.getY());

				double x, y, w, h;
				Rectangle tempTangle = new Rectangle();
				Boolean intersects = false;

				if (event.getX() > pressedX && event.getY() > pressedY) {
					// Oben links nach unten rechts
					w = event.getX() - pressedX;
					h = event.getY() - pressedY;
					x = event.getX() - w;
					y = pressedY;
				} else if (pressedX > event.getX() && pressedY > event.getY()) {
					// Unten rechts nach oben links
					w = pressedX - event.getX();
					h = pressedY - event.getY();
					x = pressedX - (pressedX - event.getX());
					y = event.getY();
				} else if (pressedX > event.getX() && pressedY < event.getY()) {
					// Oben rechts nach unten links
					w = pressedX - event.getX();
					h = event.getY() - pressedY;
					x = event.getX();
					y = event.getY() - (event.getY() - pressedY);
				} else {
					// Unten links nach oben rechts
					w = event.getX() - pressedX;
					h = pressedY - event.getY();
					x = event.getX() - (event.getX() - pressedX);
					y = pressedY - (pressedY - event.getY());
				}

				tempTangle.x = (int) x;
				tempTangle.y = (int) y;
				tempTangle.width = (int) w;
				tempTangle.height = (int) h;

				// RED PREVIEW IF ROOM INVALID
				for (Rectangle r : rectangles) {
					if (r.intersects(tempTangle)) {
						intersects = true;
						break;
					}
				}

				if (tempTangle.width < 58 || tempTangle.height < 72) {
					gc2.setStroke(Color.RED);
				} else if ((event.getX() > 554 || event.getX() < 0) || (event.getY() > 746 || event.getY() < 0)) {
					gc2.setStroke(Color.RED);
				} else if (intersects) {
					gc2.setStroke(Color.RED);
				} else {
					gc2.setStroke(Color.WHITE);
				}

				gc2.strokeRect(x, y, w, h);
				event.consume();
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
				@Override
				public void handle(MouseEvent event) {
					Dragboard db = n.startDragAndDrop(TransferMode.MOVE);
					ClipboardContent content = new ClipboardContent();
					content.putString("Hallo");
					db.setContent(content);
					if (n.getId() == list.get(0).getId()) {
						tempModulID = 1;
						// dropping modul 1
						Image image = new Image("/GUI/resources/modul1.png");
						db.setDragView(image);
					} else if (n.getId() == list.get(1).getId()) {
						tempModulID = 2;
						// dropping modul 2
						Image image = new Image("/GUI/resources/modul2.png");
						db.setDragView(image);
					} else if (n.getId() == list.get(2).getId()) {
						tempModulID = 3;
						// dropping modul 3
						Image image = new Image("/GUI/resources/modul3.png");
						db.setDragView(image);
					} else if (n.getId() == list.get(3).getId()) {
						tempModulID = 0;
						Image image = new Image("/GUI/resources/light.png");
						db.setDragView(image);
					}

					// Handling illegal drop positions (\)
					anchorpane.setOnDragOver(new EventHandler<DragEvent>() {
						@Override
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

							// Handling the drop and adding new objects to the
							// room aswell as making
							// already used modules unavailable
							anchorpane.setOnDragDropped(new EventHandler<DragEvent>() {
								@Override
								public void handle(DragEvent event) {
									Point p = new Point();
									p.setLocation(event.getX(), event.getY());

									if (tempRaum.getRect().contains(p)) {
										if (tempModulID == 0) {
											// Fï¿½GT EIN LICHT HINZU
											tempRaum.setLicht(new Licht(p, tempRaum, anchorpane));
											createLichtAnzeige(tempRaum);
											System.out.println("Licht zu Raum " + tempRaum.getID() + " hinzugefï¿½gt!");
											tempRaum = null;
										} else {
											// ERSTELLT DEN RAUM
											tempRaum.setModul(new Modul(tempModulID));
											System.out.println(
													"Modul " + tempRaum.getModul().getModulID() + " hinzugefï¿½gt!");
											// Fï¿½GT TEMPERATURANZEIGE HINZU
											createTempAnzeige(tempRaum);
											// MALE RAUM AUS
											gc.setFill(Color.WHITE);
											gc.fillRect(tempRaum.getRect().getX() + 1, tempRaum.getRect().getY() + 1,
													tempRaum.getRect().getWidth() - 2,
													tempRaum.getRect().getHeight() - 2);
											// DEAKTIVIERE BEREITS BENUTZTE
											// MODULE
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

							event.consume();
						}
					});

					event.consume();
				}
			});
		}

		/*
		 * reset Button
		 */
		reset = (Button) anchorpane.getChildren().get(0);

		reset.setOnMouseClicked(new EventHandler<MouseEvent>() {
			@Override
			public void handle(MouseEvent event) {
				// clear the canvas.
				gc.clearRect(0, 0, 554, 746);
				gc2.clearRect(0, 0, 554, 746);

				// remove all elements of the lists
				raumListe.clear();
				rectangles.clear();

				// Reset the room counter
				idCounter = 1;

				// Remove the Light and Temperature Buttons
				anchorpane.getChildren().subList(3, anchorpane.getChildren().size()).clear();

				// reset all module's visibilties.
				for (Node n : list) {
					if (n.isDisable() == true) {
						n.setDisable(false);
						n.setOpacity(1);
					}
				}
			}
		});
		// set the primaryStage once
		SingletonClass.getSingletonInstanz().setStage(primaryStage);

		primaryStage.setScene(scene);

		// Set the title of the stage
		primaryStage.setTitle("SmartAssist");

		// locks window
		primaryStage.setResizable(false);

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
			r.getKlima().setImageAndLabel(temperatur);

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

	/**
	 * Verarbeitet den vom WSN erhaltenen String
	 * 
	 * @return
	 */
	public void updateRoom(String raumString) {
		System.out.println("Bekomme:");
		System.out.println(raumString);
		String[] raumStringArray = raumString.split(";");

		// "ID;Status;Mode_Light;Mode_Climate;LED_Status;Illuminance;Temperature"
		// Daten für Raum auslesen
		int raumId = Integer.parseInt(raumStringArray[0]);
		boolean lichtModus = "1".equals(raumStringArray[2]);
		boolean tempStatus = "1".equals(raumStringArray[3]);
		boolean lichtStatus = "1".equals(raumStringArray[4]);
		int lichtWert = Integer.parseInt(raumStringArray[5]);
		float tempWert = Float
				.parseFloat(raumStringArray[6].substring(0, 2) + "." + raumStringArray[6].substring(2, 4));

		// Werte dem entsprechenden Raum zuweisen
		for (Raum r : raumListe) {
			if (r.getModul() != null && r.getModul().getModulID() == raumId) {
				r.getModul().setlichtwert(lichtWert);
				//r.getModul().settemperatur(tempWert);
				System.out.println("Lichtstatus:");
				System.out.println(raumStringArray[4]);
				System.out.println(lichtStatus);
				r.getKlima().setHeizungsstatus(tempStatus);
				r.getModul().settemperatur(tempWert);
				r.getKlima().setImageAndLabel(r.getModul().temperaturanzeige((float)r.getKlima().getZielTemp()));
				if(r.getLicht() != null) {
					r.getLicht().setLichtModus(lichtModus);
					r.getLicht().setLichtAnAus(lichtStatus);
				}
				break;
			}
		}

	}

	/**
	 * Sendet die aktuellen Daten und Einstellungen für ein Modul an das WSN
	 * 
	 * @param Raum
	 *            der Raum für den das Modul mit allen Einstellungen im WSN
	 *            übernommen werden soll
	 */
	public static void updateModule(Raum raum) {
		// "ID;Status;Mode_Light;Mode_Climate;LED_Status;Illuminance_Reference;Temperature_ReferenceE"
		if (raum.getModul() != null) {
			String moduleId = Integer.toString(raum.getModul().getModulID()) + ";";
			String moduleStatus = "1;";

			String lichtModus;
			String tempStatus;
			String lichtStatus;
			String lichtZiel = "000;";

			if (raum.getLicht() != null) {
				if (raum.getLicht().getLichtModus()) {
					lichtModus = "1;";
				} else {
					lichtModus = "0;";
				}

				if (raum.getLicht().getLichtAnAus()) {
					lichtStatus = "1;";
				} else {
					lichtStatus = "0;";
				}

				// LichtZiel von 0-3 in 0-255 übersetzen
				if (raum.getLicht().getLichtZielWert() == 0) {
					lichtZiel = ("000;");
				}
				if (raum.getLicht().getLichtZielWert() == 1) {
					lichtZiel = ("085;");
				}
				if (raum.getLicht().getLichtZielWert() == 2) {
					lichtZiel = ("170;");
				}
				if (raum.getLicht().getLichtZielWert() == 3) {
					lichtZiel = ("255;");
				}

			} else {
				lichtStatus = "0;";
				lichtModus = "0;";
			}

			if (raum.getKlima().getHeizungsstatus()) {
				tempStatus = "1;";
			} else {
				tempStatus = "0;";
			}

			// Format für die ZielTemperatur festlegen
			DecimalFormat df = new DecimalFormat("00.00");
			String tempZielFormat = df.format(raum.getKlima().getZielTemp());
			String tempZiel = tempZielFormat.substring(0, 2) + tempZielFormat.substring(3, 5) + "E";
			System.out.println(moduleId + moduleStatus + lichtModus + tempStatus + lichtStatus + lichtZiel + tempZiel);
			schnittstelle.send(moduleId + moduleStatus + lichtModus + tempStatus + lichtStatus + lichtZiel + tempZiel);
			//schnittstelle.close();
		}
	}

	public static ArrayList<Raum> getRaumListe() {
		return raumListe;
	}

	public static void setRaumListe(ArrayList<Raum> raumListe) {
		Gui.raumListe = raumListe;
	}
}
