// EventHandler.java 

import	java.util.List;
import	java.util.ArrayList;
import	java.util.Iterator;

public class EventHandler {
	public static interface Observer {
		public void		OnUpdate();
	}

	public static abstract class Subject {
		final private List<Observer>	list = new ArrayList<Observer>();

		public void			addObserver( Observer observer ){
			list.add( observer );
		}

		public void			delObserver( Observer observer ){
			int idx = list.indexOf( observer );
			list.remove( idx );
		}

		public void			notifyObserver(){
			Iterator it = list.iterator();
			while( it.hasNext() ){
				((Observer)it.next()).OnUpdate();
			}
		}	
	}
}
