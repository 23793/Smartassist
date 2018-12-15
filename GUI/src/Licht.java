package GUI.src;

import java.awt.Point;

import javafx.event.ActionEvent;
import javafx.event.Event;
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

public class Licht {

	private Raum raum;

	private boolean lichtStatus = false;
	private Point lichtPoint;

	private ImageView anBild = new ImageView(new Image("/GUI/resources/idea.png", true));
	private ImageView ausBild = new ImageView(new Image("/GUI/resources/lampe.png", true));

	private Button settings = new Button(null, ausBild);

	private VBox vebox = new VBox();
	private HBox box = new HBox();

	public Licht(Point p, Raum r, AnchorPane ap) {
		lichtPoint = p;
		raum = r;

		anBild.setFitHeight(40);
		anBild.setFitWidth(40);
		ausBild.setFitHeight(40);
		ausBild.setFitWidth(40);

		settings.setBackground(null);

		vebox.setLayoutX(lichtPoint.x - 28);
		vebox.setLayoutY(lichtPoint.y - 30);

		// Clickable icon for LichtPopup
		settings.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent e) {
				toggle();
			}
		});

		/*
		 * EVENTHANDLER FÜR REPOSITIONING
		 */
		settings.setOnDragDetected(new EventHandler<MouseEvent>() {
			public void handle(MouseEvent event) {
				System.out.println("Picked up Lamp");
				Dragboard db = settings.startDragAndDrop(TransferMode.MOVE);
				ClipboardContent content = new ClipboardContent();
				content.putString("Hallo");
				db.setContent(content);

				ap.setOnDragOver(new EventHandler<DragEvent>() {
					public void handle(DragEvent event) {
						System.out.println("dragging light");
						Point p = new Point();
						p.setLocation(event.getX(), event.getY());
						if (r.getRect().contains(p)) {
							event.acceptTransferModes(TransferMode.MOVE);
						}

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

		ap.setOnDragDropped(new EventHandler<DragEvent>() {
			public void handle(DragEvent event) {
				Point p = new Point();
				p.setLocation(event.getX() - 28, event.getY() - 30);
				setLichtPoint(p);
				event.setDropCompleted(true);
				event.consume();
			}
		});
	}

	public void toggle() {
		lichtStatus = !lichtStatus;

		if (lichtStatus) {
			settings.setGraphic(anBild);
			System.out.println("Licht an");
		} else {
			settings.setGraphic(ausBild);
			System.out.println("Licht aus");

		}
	}

	public void setLichtStatus(boolean state) {
		lichtStatus = state;
		if (lichtStatus) {
			settings.setGraphic(anBild);
			System.out.println("Licht an");
		} else {
			settings.setGraphic(ausBild);
			System.out.println("Licht aus");

		}
	}

	public boolean getLichtStatus() {
		return lichtStatus;
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

}
