// Main.java 

public class Main {
	public static void	main( String[] args ){
		javax.swing.SwingUtilities.invokeLater( new Runnable() {
			public void		run(){
				new RenderFrame( 100, 100, 600, 600 ).setVisible( true );
				new ParameterFrame( 700, 100, 500, 600 ).setVisible( true );
			}
		});
	}
}