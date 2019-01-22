package GUI.src;

import java.awt.Point;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
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
import javafx.stage.Stage;

/**
 * The Licht class contains all logical functionalities and graphical displays
 * of lights in rooms.
 * 
 * @author MinhMax
 *
 */
public class Licht {

	private Raum raum;

	// ObservableBoolean used in the light pop-up
	private Boolean lichtAnAus = false;
	private boolean lichtModus = false; // Default manual
	private int lichtZielWert = 2;
	private Point lichtPoint;

	private ImageView anBild = new ImageView(new Image("/GUI/resources/idea.png", true));
	private ImageView ausBild = new ImageView(new Image("/GUI/resources/bw.png", true));

	private Button settings = new Button(null, ausBild);

	private VBox vebox = new VBox();
	private HBox box = new HBox();

	// Constructor with position, room, and AnchorPane for display
	public Licht(Point p, Raum r, AnchorPane ap) {
		lichtPoint = p;
		raum = r;

		// Set icon scale
		anBild.setFitHeight(40);
		anBild.setFitWidth(40);
		ausBild.setFitHeight(40);
		ausBild.setFitWidth(40);

		settings.setBackground(null);

		// Set the position to the middle of the icon
		vebox.setLayoutX(lichtPoint.x - 28);
		vebox.setLayoutY(lichtPoint.y - 30);

		// Clickable icon for light pop-up
		settings.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent e) {
				Lichtpop lp = new Lichtpop(raum);
				Stage stage = new Stage();
				stage.setTitle("Lichtkonfiguartion");
				try {
					lp.display(stage);
				} catch (Exception e1) {
					e1.printStackTrace();
				}
			}
		});

		/*
		 * Event handler for light repositioning (drag&drop)
		 */
		settings.setOnDragDetected(new EventHandler<MouseEvent>() {
			public void handle(MouseEvent event) {
				Dragboard db = settings.startDragAndDrop(TransferMode.MOVE);
				ClipboardContent content = new ClipboardContent();
				content.putString("Hallo");
				Image image = new Image("/GUI/resources/bw2.png");
				db.setDragView(image);
				db.setContent(content);

				// Handling illegal drop positions
				ap.setOnDragOver(new EventHandler<DragEvent>() {
					public void handle(DragEvent event) {
						Point p = new Point();
						p.setLocation(event.getX(), event.getY());
						if (r.getRect().contains(p)) {
							event.acceptTransferModes(TransferMode.MOVE);
						}

						// Dropping the light icon
						ap.setOnDragDropped(new EventHandler<DragEvent>() {
							public void handle(DragEvent event) {
								Point p = new Point();
								p.setLocation(event.getX() - 28, event.getY() - 30);
								setLichtPoint(p);
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

	public Boolean getLichtBoolean() {
		return lichtAnAus;
	}

	public void setLichtAnAus(boolean state) {
		lichtAnAus = state;
		if (lichtAnAus) {
			settings.setGraphic(anBild);
		} else {
			settings.setGraphic(ausBild);
		}
	}

	public boolean getLichtAnAus() {
		return lichtAnAus;
	}

	public void setLichtPoint(Point p) {
		lichtPoint = p;
		vebox.setLayoutX(lichtPoint.x);
		vebox.setLayoutY(lichtPoint.y);
	}

	public Point getLichtPoint() {
		return lichtPoint;
	}

	public Button getSettings() {
		return settings;
	}

	public void setSettings(Button settings) {
		this.settings = settings;
	}

	public VBox getVebox() {
		return vebox;
	}

	public void setVebox(VBox vebox) {
		this.vebox = vebox;
	}

	public HBox getBox() {
		return box;
	}

	public void setBox(HBox box) {
		this.box = box;
	}

	public Raum getRaum() {
		return raum;
	}

	public void setRaum(Raum raum) {
		this.raum = raum;
	}

	public int getLichtZielWert() {
		return lichtZielWert;
	}

	public void setLichtZielWert(int lichtZielWert) {
		this.lichtZielWert = lichtZielWert;
	}

	public boolean getLichtModus() {
		return lichtModus;
	}

	public void setLichtModus(boolean lichtModus) {
		this.lichtModus = lichtModus;
	}

}
