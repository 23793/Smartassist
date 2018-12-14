package GUI.src;

import javafx.scene.control.Button;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class Klima {

	private boolean heizungsstatus;
	private boolean ACstatus;
	
	Image fire = new Image("/GUI/resources/fire.png", true);
	Image temp = new Image("/GUI/resources/temp.png", true);
	Image snow = new Image("/GUI/resources/snow.png", true);
	Image perfect = new Image("/GUI/resources/perfect.png", true);

	ImageView iv1 = new ImageView();
	ImageView iv2 = new ImageView();
	
	public Klima() {
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
}
