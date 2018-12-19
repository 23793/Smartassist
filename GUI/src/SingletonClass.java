package GUI.src;

import javafx.stage.Stage;

public class SingletonClass {
	/*
	 * Statisches attribut von stage.
	 */
	private static Stage stage;
	/*
	 * Statische Instanz von Typ SingletonStage.
	 */
	private static SingletonClass singletonInstanz;
	
	/*
	 * default Konstruktor.
	 * Kein Zugriff von Außen.
	 */
	private SingletonClass(){}
	
	/*
	 * statische synchronizierte Methode
	 * 
	 */
	public synchronized static SingletonClass getSingletonInstanz()
	{
		if(singletonInstanz == null)
		{
			singletonInstanz = new SingletonClass();
		}
		return singletonInstanz;
	}
	
	/*
	 * setter Methode von Typ Stage.
	 */
	public Stage getStage()
	{
		return stage;
	}
	
	public void setStage(Stage stage)
	{
		/* da this.stage statisch ist, 
		 * muss zuerst die SingletonStage Klasse 
		 * aufgerufen werden
		 */
		SingletonClass.stage = stage;
	}
}
