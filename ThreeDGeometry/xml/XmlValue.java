// XmlValue.java 
//package ThreeDGeometry.xml;

import	java.io.*;
import	java.util.*;
import	javax.xml.parsers.*;
import	javax.xml.transform.*;
import	javax.xml.transform.dom.*;
import	javax.xml.transform.stream.*;
import	org.w3c.dom.*;

class XmlMap {
	private static Map<String,String>	m_map = new HashMap<String,String>();
	public static void		add( String key, String value ){
		m_map.put( key, value );
	}
	public static void		add( String key, int value ){
		m_map.put( key, Integer.toString(value) );
	}
	public static void		add( String key, double value ){
		m_map.put( key, Double.toString(value) );
	}
	public static String	find( String key ){
		if( m_map.containsKey(key) ){
			return m_map.get(key);
		}
		return "";
	}

	public static void		trace(){
		for( Map.Entry<String,String> e : m_map.entrySet() ){
			System.out.println( "[" + e.getKey() + "]: " + e.getValue() );
		}
	}

	public static void		write(){
		try{
			DocumentBuilderFactory	factory	= DocumentBuilderFactory.newInstance();
			DocumentBuilder			builder	= factory.newDocumentBuilder();

	//		DOMImplementation		DOMImp	= builder.getDOMImplementation();
	//		Document				doc		= DOMImp.createDocument( "", "root", null );
			Document				doc		= builder.newDocument();

			Element root = doc.createElement( "Camera" );
			doc.appendChild( root );

			for( Map.Entry<String,String> e : m_map.entrySet() ){
				Element key = doc.createElement( e.getKey() );
				key.appendChild( doc.createTextNode( e.getValue() ) );
				root.appendChild( key );
			}

			TransformerFactory		transfactory	= TransformerFactory.newInstance();
			Transformer				trans			= transfactory.newTransformer();

			File			fileXML	= new File( "param.xml" );
			FileOutputStream	os	= new FileOutputStream( fileXML );

			trans.transform( new DOMSource(doc), new StreamResult(os) );
		}
		catch( ParserConfigurationException e ){
			System.out.println( "newDocumentBuilder()" );
		}
		catch( TransformerConfigurationException e ){
			System.out.println( "newTransformer()" );
		}
		catch( FileNotFoundException e ){
			System.out.println( "new FileOutputStream()" );
		}
		catch( TransformerException e ){
			System.out.println( "transform()" );
		}
	}
} 

public class XmlValue {
	public static void	main(String[] args){
		XmlMap.add( "EyeX", "100" );
		XmlMap.add( "EyeY", "100" );
		XmlMap.add( "EyeZ", "100" );
		XmlMap.add( "LookAtX", "100" );
		XmlMap.add( "LookAtY", "100" );
		XmlMap.add( "LookAtZ", "100" );
		XmlMap.add( "EyeX", 100.0 );
		XmlMap.add( "EyeY", 1.234 );
		XmlMap.add( "EyeZ", "100" );
		
		XmlMap.trace();
		XmlMap.write();
	}
}
