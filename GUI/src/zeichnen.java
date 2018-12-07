package application;

import javafx.event.EventHandler;
import javafx.geometry.Bounds;

import java.awt.Graphics;
import java.util.ArrayList;

import javafx.application.Application;
import javafx.stage.Stage;
//import javafx.scene.Group;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.Label;
import javafx.scene.input.*;
//import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import java.awt.Rectangle;
//import javafx.scene.paint.Color;
//import javafx.scene.shape.Rectangle;
//import javafx.scene.shape.Shape;
import javafx.scene.shape.Shape;

public class Main extends Application {

	double pressedX;
	double pressedY;
	double releasedX;
	double releasedY;
	int countgc = 0; // Counter, der jeden erstellten Raum zählt
	static ArrayList<Rectangle> raumliste = new ArrayList<Rectangle>();

	public void start(Stage primaryStage) throws Exception {

		VBox root = new VBox();
		Scene scene = new Scene(root);
		Canvas canvas = new Canvas(700, 700);
		GraphicsContext gc = canvas.getGraphicsContext2D();

		Label label = new Label("Grundriss zeichnen");
		root.getChildren().add(label);
		root.getChildren().add(canvas);

		/**
		 * X, Y Werte, wenn Maus gedrückt ist
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

		primaryStage.setScene(scene);
		primaryStage.show();
	}

	private void drawRectangle(GraphicsContext gc) {

		// Höhe und Breite berechnen
		double breite = releasedX - pressedX;
		double hoehe = releasedY - pressedY;

	//	 gc.setLineDashes(0); //Breite der Linie
	//	 gc.setStroke(Color.HOTPINK);

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

	public static void main(String[] args) {
		launch(args);
	}
}
