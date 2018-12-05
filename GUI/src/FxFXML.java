package smartassist.view;

import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;

import java.io.FileInputStream;

public class FxFXML extends Application {

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {

        //create the FXML Loader
        FXMLLoader loader = new FXMLLoader();

        /*
         * create the AnchorPane and all details and
         * create the AnchorPane and all details and
         * load the Path of the FXML File
         * */
        AnchorPane anchorPaneRoot = (AnchorPane)FXMLLoader.load(getClass().getResource("RoomView.fxml"));

        // create the scene
        Scene scene = new Scene(anchorPaneRoot);

        //Set the scene to the stage
        primaryStage.setScene(scene);

        // Set the title of the stage
        primaryStage.setTitle("Smart");

        //Display the stage
        primaryStage.show();

    }

}
