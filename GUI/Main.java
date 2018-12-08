package GUI;

import java.awt.Rectangle;
import java.util.ArrayList;

import javafx.scene.Node;

import javafx.application.Application;
import javafx.collections.ObservableList;
import javafx.event.EventHandler;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.SplitPane;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class Main extends Application{
//	private static Schnittstelle s = new Schnittstelle();
//	private static Raum r1 = new Raum(), r2 = new Raum(), r3 = new Raum();
//
//	public static void main(String[] args) {
//		s.connect();
//
//		s.send("Boi.");
//		s.receive();
//
//		s.close();
//	}
//
//	private void updateRooms() {
//		String roomString;
//		String roomAttributes[];
//		Raum t1, t2, t3;
//
//		s.connect();
//		roomString = s.receive(); // Gets the room data String
//		s.close();
//
//		roomAttributes = roomString.split(";");
//
//		// "id1;temp1;light1;switch1;id2;temp2;light2;switch2;id3;temp3;light3;switch3;
//
//		// Creating temp rooms
//		t1 = new Raum(Integer.parseInt(roomAttributes[0]), Float.parseFloat(roomAttributes[1]),
//				Integer.parseInt(roomAttributes[2]), Integer.parseInt(roomAttributes[3]));
//		t2 = new Raum(Integer.parseInt(roomAttributes[4]), Float.parseFloat(roomAttributes[5]),
//				Integer.parseInt(roomAttributes[6]), Integer.parseInt(roomAttributes[7]));
//		t3 = new Raum(Integer.parseInt(roomAttributes[8]), Float.parseFloat(roomAttributes[9]),
//				Integer.parseInt(roomAttributes[10]), Integer.parseInt(roomAttributes[11]));
//
//		// Muss hier noch eine Logik hin?
//		r1 = t1;
//		r2 = t2;
//		r3 = t3;
//	}
    @FXML
    private Canvas canvas1;
	
	double pressedX;
	double pressedY;
	double releasedX;
	double releasedY;
	static ArrayList<Rectangle> raumliste = new ArrayList<Rectangle>();
	
	
	
	public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {

        /*
         * create the AnchorPane and all details and
         * load the Path of the FXML File
         * */
        AnchorPane anchorPaneRoot = (AnchorPane)FXMLLoader.load(getClass().getResource("GUI/src/RoomView.fxml"));
        
//        System.out.println(anchorPaneRoot.getChildren());
//        ObservableList<Node> obs = anchorPaneRoot.getChildren();
        
//       AnchorPane ap = new AnchorPane(obs.get(1));
       
       
       ObservableList<Node> obs = anchorPaneRoot.getChildren();
       
////   sp.get(0)
       SplitPane sp = (SplitPane) obs.get(0);
       AnchorPane rightAnchorPane = (AnchorPane) sp.getItems().get(1);
       
       
//		VBox vbox = new VBox();
//		Scene scene = new Scene(vbox);
		Scene scene = new Scene(anchorPaneRoot);
		this.canvas1 = new Canvas(rightAnchorPane.getWidth(), rightAnchorPane.getHeight() );
		Canvas canvas = canvas1;
		GraphicsContext gc = canvas.getGraphicsContext2D();
		
		rightAnchorPane.getChildren().add(canvas);
//		rightAnchorPane.getChildren().add(vbox);
		
		

       
//       ap.getChildren().addAll();
       
       
       /**
		 * X, Y Werte, wenn Maus gedr�ckt ist
		 *
		 */
		scene.setOnMousePressed(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				pressedX = event.getX();
				pressedY = event.getY();
				System.out.println("X = " + pressedX);
				System.out.println("Y = " + pressedY);
			}
		});

		/**
		 * X, Y Werte, wenn Maus losgelassen wird
		 *
		 */
		scene.setOnMouseReleased(new EventHandler<MouseEvent>() {

			public void handle(MouseEvent event) {
				releasedX = event.getX();
				releasedY = event.getY();
				System.out.println("Release X = " + releasedX);
				System.out.println("Release Y = " + releasedY);
				drawRectangle(gc);
			}
		});

//		primaryStage.setScene(scene);
		//primaryStage.show();
       
       
       
       
        // create the scene
        

        //Set the scene to the stage
        primaryStage.setScene(scene);

        // Set the title of the stage
        primaryStage.setTitle("Smart");

        //Display the stage
        primaryStage.show();

    }
    
    private void drawRectangle(GraphicsContext gc) {

		// H�he und Breite berechnen
		double breite = releasedX - pressedX;
		double hoehe = releasedY - pressedY;

	//	 gc.setLineDashes(0); //Breite der Linie
		 gc.setStroke(Color.RED);

		Rectangle viereck = new Rectangle();
		viereck.x = (int) pressedX;
		viereck.y = (int) pressedY;
		viereck.width = (int) breite;
		viereck.height = (int) hoehe;



		Boolean intersect = false;
		for (Rectangle r : raumliste) {
			if (r.intersects(viereck)) {
				intersect = true;
				break;
			}
		}
		if (!intersect) {
			gc.strokeRect(pressedX, pressedY, breite, hoehe);
			raumliste.add(viereck);

		}

		System.out.println("Raumliste:" + raumliste.size());
		System.out.println("Breite: " + breite);
		System.out.println("Höhe: " + hoehe);

	}
	
	
}

/*
 * GUI �ber Thread laufen lassen
 */
