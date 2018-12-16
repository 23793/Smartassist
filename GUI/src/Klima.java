package GUI.src;

import java.awt.Point;

import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class Klima {

	private boolean heizungsstatus;
	private boolean ACstatus;

	private Image fire = new Image("/GUI/resources/fire.png", true);
	private Image temp = new Image("/GUI/resources/temp.png", true);
	private Image snow = new Image("/GUI/resources/snow.png", true);
	private Image perfect = new Image("/GUI/resources/perfect.png", true);

	private ImageView iv1 = new ImageView();
	private ImageView iv2 = new ImageView();

	private Label temps = new Label();

	private Button settings = new Button(null, new ImageView(temp));

	/*
	 * Vbox und Hbox für Temperaturanzeige
	 */
	private VBox vebox = new VBox();
	private HBox box = new HBox();

	public Klima(Point p) {
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
		settings.setBackground(null);

		/*
		 * Position der Temperaturanzeige HIER: ersetzten mit X und Y Werten
		 * eines Raumes.
		 */
		vebox.setLayoutX(p.x - 71);
		vebox.setLayoutY(p.y - 80);

		// Clickable icon for TempPopup
		settings.setOnAction(new EventHandler<ActionEvent>() {
			@Override
			public void handle(ActionEvent e) {
				// TODO Auto-generated method stub
				System.out.println("Test");
			}
		});
	}

	public void setImageAndLabel(String s, String temp) {
		/**
		 * checkt den wert von s und gibt entsprechendes Bild aus. Setzt
		 * außerdem die Temperatur für das Label fest.
		 */
		if (s.equals("kalt")) {
			iv1.setImage(snow);
		} else if (s.equals("heiss")) {
			iv1.setImage(fire);
		} else if (s.equals("perfekt")) {
			iv1.setImage(perfect);
		}
		temps.setText(temp + "°C");
	}

	public void set_heizungsstatus(boolean status) {
		heizungsstatus = status;
	}

	public boolean get_heizungsstatus() {
		return heizungsstatus;
	}

	public void set_ACstatus(boolean status) {
		ACstatus = status;
	}

	public boolean get_ACstatus() {
		return ACstatus;
	}

	public ImageView getIv1() {
		return iv1;
	}

	public void setIv1(ImageView iv1) {
		this.iv1 = iv1;
	}

	public ImageView getIv2() {
		return iv2;
	}

	public void setIv2(ImageView iv2) {
		this.iv2 = iv2;
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

	public Button getSettings() {
		return settings;
	}

	public void setSettings(Button settings) {
		this.settings = settings;
	}

	public Label getTemps() {
		return temps;
	}

	public void setTemps(Label temps) {
		this.temps = temps;
	}
}
