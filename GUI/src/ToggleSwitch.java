package GUI.src;

import javafx.beans.property.SimpleBooleanProperty;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;

	//automatisch = true = 1
	//manuell = false = 0

	public class ToggleSwitch extends HBox {

		private final Label label = new Label();
		private final Button button = new Button();
		private int modus = 1; //default true = automatisch

		private SimpleBooleanProperty switchedOn = new SimpleBooleanProperty(false);
		public SimpleBooleanProperty switchOnProperty() { return switchedOn; }

		private void init() {

		//	label.setText("OFF");

			getChildren().addAll(label, button);
			button.setOnAction((e) -> {
				switchedOn.set(!switchedOn.get());
				setmodus(true); //modus wird automatisch (1) gesetzt
			});
			label.setOnMouseClicked((e) -> {
				switchedOn.set(!switchedOn.get());
				setmodus(false); //modus wird manuell (0) gesetzt
			});
			setStyle();
			bindProperties();
		}

		public void setmodus(boolean b) {
			if(b == true) {
				modus = 0;
				System.out.println("manuell");
			}
				else {
					System.out.println("auto");
					modus = 1;
				}
			}

			public int get_modus() {
				return modus;
			}

		private void setStyle() {
			//Default Width
			setWidth(80);
			label.setAlignment(Pos.CENTER);
			setStyle("-fx-background-color: grey; -fx-text-fill:black; -fx-background-radius: 4;");
			setAlignment(Pos.CENTER_LEFT);
		}

		private void bindProperties() {
			label.prefWidthProperty().bind(widthProperty().divide(2));;
			label.prefHeightProperty().bind(heightProperty());
			button.prefWidthProperty().bind(widthProperty().divide(2));
			button.prefHeightProperty().bind(heightProperty());
		}

		public ToggleSwitch() {
			init();
			switchedOn.addListener((a,b,c) -> {
				if (c) {
	                	//	label.setText("ON");
	                		setStyle("-fx-background-color: green;");
	                		label.toFront();
	            		}
	            		else {
	            		//	label.setText("OFF");
	        			setStyle("-fx-background-color: grey;");
	                		button.toFront();
	            		}
			});
		}
	}


