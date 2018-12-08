package GUI.src;

import java.io.IOException;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Scene;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;

public class Main2 extends Application {

	@Override
	public void start(Stage primaryStage) throws IOException {
		/*
         * create the AnchorPane and all details and
         * load the Path of the FXML File
         * */
        AnchorPane anchorPaneRoot = (AnchorPane)FXMLLoader.load(getClass().getResource("RoomView.fxml"));
        
        System.out.println(anchorPaneRoot.getChildren());
        
//        ObservableList<Node> sp = anchorPaneRoot.getChildren();
////        sp.get(0)
//        SplitPane sp1 = (SplitPane) sp.get(0);
//        
//        System.out.println(sp1.getWidth());
//        System.out.println(sp1.getHeight());
//        System.out.println(sp1.getItems().size());
//        System.out.println(sp1.getItems().get(0));

        // create the scene
        Scene scene = new Scene(anchorPaneRoot);

        //Set the scene to the stage
        primaryStage.setScene(scene);

        // Set the title of the stage
        primaryStage.setTitle("Smart");

        //Display the stage
        primaryStage.show();
	}

	public static void main(String[] args) {
		launch(args);
	}
}
