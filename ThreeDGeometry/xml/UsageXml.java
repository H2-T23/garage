// UsageXml.java 

/***
「W3C DOM 3 XPathの勧告に従った方法でXPathを評価する」
 */

/*
import	org.w3c.dom.DOMImplementation;
import	org.w3c.dom.Document;
import	org.w3c.dom.bootstrap.DOMImplementationRegistry;
import	org.w3c.dom.ls.DOMImplementationLS;
import	org.w3c.dom.ls.LSInput;
import	org.w3c.dom.ls.LSParser;
import	org.w3c.dom.xpath.XPathEvaluator;
import	org.w3c.dom.xpath.XPathExpression;
import	org.w3c.dom.xpath.XPathResult;
*/

import	java.io.*;

import	org.w3c.dom.CharacterData;
import	org.w3c.dom.Document;
import	org.w3c.dom.Element;
import	org.w3c.dom.Node;
import	org.w3c.dom.NodeList;
import	org.w3c.dom.traversal.NodeIterator;

import	org.xml.sax.InputSource;


import	javax.xml.parsers.DocumentBuilder;
import	javax.xml.parsers.DocumentBuilderFactory;

import	javax.xml.transform.Result;
import	javax.xml.transform.Source;
import	javax.xml.transform.Transformer;
import	javax.xml.transform.TransformerFactory;
import	javax.xml.transform.OutputKeys;

import	javax.xml.transform.dom.DOMSource;
import	javax.xml.transform.dom.DOMResult;

import	javax.xml.transform.stream.StreamSource;
import	javax.xml.transform.stream.StreamResult;

import	javax.xml.xpath.XPath;
import	javax.xml.xpath.XPathConstants;
import	javax.xml.xpath.XPathFactory;

import	javax.xml.parsers.ParserConfigurationException;

/*
<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
	xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
	xmlns:xalan="http://xml.apache.org/xsls">
	<xsl:output method="xml" encoding="UTF-8" indent="yes" xalan:indent-amount="2"/>
	<xsl:template match="/">
		<xsl:copy-of select="."/>
	</xsl:template>
</xsl:stylesheet>
*/
	
public class UsageXml {

	final public static String				strXSL = 
	"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
+	"<xsl:stylesheet version=\"1.0\" xmlns:xsl=\"http://www.w3.org/1999/XSL/Transform\" xmlns:xalan=\"http://xml.apache.org/xslt\">"
+		"<xsl:output method=\"xml\" encoding=\"UTF-8\" indent=\"yes\" xalan:indent-amount=\"2\"/>"
+		"<xsl:template match=\"/\">"
+			"<xsl:copy-of select=\".\"/>"
+		"</xsl:template>"
+	"</xsl:stylesheet>"
;


	static class UsageDOM {
		/***
		[JavaでXML文書を読み込んでDOMツリーを構築するサンプル]
		XML文書をツリー形式で扱うためのモデルとしてDOMがあり、W3Dがその仕様を公開しています。
		例えば、DOM 3 Core Specification などです。

			DOM Load and Save(http://www.w3.org/ * /load-save.html)

		

		 */

		/**********************************************************************
		 *
		 */
		public UsageDOM(){
			ParameterXml();
		}

		/**********************************************************************
		 *
		 */
		public void			ParameterXml(){
			doc = newDocument( "Parameters" );
			if( doc != null ){
				Element root = doc.getDocumentElement();
				System.out.println( "root element type: " + root.getNodeType() );
				System.out.println( "root element name: " + root.getNodeName() );
				
			//	doc.appendChild( root );

				Element struct	= doc.createElement( "stract" );
				Element member	= doc.createElement( "member" );
				Element type	= doc.createElement( "type"   );
				Element value	= doc.createElement( "value"  );
				
				type.appendChild( doc.createTextNode( "int" ) );
				value.appendChild( doc.createTextNode( "123" ) );

				member.appendChild( type );
				member.appendChild( value );

				struct.appendChild( member );

				root.appendChild( struct );
				

				displayConsole();
			//	saveFile( "in.xml" );
			//	XSLTransformation( "in.xml", "out.xml" );
			}
		}

		/**********************************************************************
		 *
		 */
		public void			XSLTransformation( String strSrcXmlFilename, String strDstXmlFilename ){
			File inXml	= new File( strSrcXmlFilename );
			File outXml	= new File( strDstXmlFilename );
			File xsl	= new File( "style.xsl" );

			try{
				TransformerFactory factory = TransformerFactory.newInstance();
				Source xslSource = new StreamSource( xsl );
				Transformer transformer = factory.newTransformer( xslSource );

				Source src = new StreamSource( inXml );
				Result ret = new StreamResult( outXml );

				transformer.transform( src, ret );
			}
			catch( Exception e ){
				e.printStackTrace();
			}
		}

		/**********************************************************************
		 *
		 */
		public Document		doc = null;


		/**********************************************************************
		 *
		 */
		public void			displayConsole(){
			try{
				TransformerFactory transFactory = TransformerFactory.newInstance();
			//	transFactory.setAttribute( "INDENT", "2" );

				Transformer	transformer = transFactory.newTransformer();
			//	transformer.setOutputProperty( OutputKeys.ENCODING, strEncoding );
				transformer.setOutputProperty( OutputKeys.INDENT,"yes" );
				transformer.setOutputProperty( OutputKeys.METHOD, "xml" );
			//	transformer.setOutputProperty( com.sun.org.apache.xml.internal.serializer.OutputPropertiesFactory.S_KEY_INDENT_AMOUNT, "2" );
			//	transformer.setOutputProperty( "xalan:indent-amount", "2" );


				DOMSource src = new DOMSource( doc );

				StreamResult output = new StreamResult( System.out );

				transformer.transform( src, output );
			}
			catch( Exception e ){
				e.printStackTrace();
			}		
		}

		/**********************************************************************
		 *
		 */
		public DocumentBuilder		newDocumentBuilder() throws ParserConfigurationException {
			return DocumentBuilderFactory.newInstance().newDocumentBuilder();
		}

		/**********************************************************************
		 *
		 */
		public Document				newDocument() throws ParserConfigurationException {
			return DocumentBuilderFactory.newInstance().newDocumentBuilder().newDocument();
		}

		/**********************************************************************
		 *
		 */
		public Document		newDocument( String strRoot ){
			try{
				Document document = DocumentBuilderFactory.newInstance().newDocumentBuilder().newDocument();
				Element root = document.createElement( strRoot );
				document.appendChild( root );
				return document;
			}
			catch( Exception e ){
				e.printStackTrace();
			}
			return null;
		}

		/**********************************************************************
		 *
		 */
		public Document		load( String strXML ){
			try{
				InputSource in = new InputSource( new StringReader( strXML ) );

				DocumentBuilder	builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
				return builder.parse( in );
			}
			catch( Exception e ){
				e.printStackTrace();
			}
			return null;
		}

		/**********************************************************************
		 *
		 */
		public Document		laodFile( String strFilename ){
			try{
				DocumentBuilder	builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
				return builder.parse( new File( strFilename ) );
			}
			catch( Exception e ){
				e.printStackTrace();
			}
			return null;
		}

		/**********************************************************************
		 *
		 */
		public void			saveFile( String strFilename ){
			this.saveFile( strFilename, "UTF-8" );
		}

		/**********************************************************************
		 *
		 */
		public void			saveFile( String strFilename, String strEncoding ){
			try{
				TransformerFactory transFactory = TransformerFactory.newInstance();
			//	transFactory.setAttribute( OutputKeys.INDENT, "2" );

				Transformer	transformer = transFactory.newTransformer();
				transformer.setOutputProperty( OutputKeys.ENCODING, strEncoding );
				transformer.setOutputProperty( OutputKeys.INDENT,"yes" );
				transformer.setOutputProperty( OutputKeys.METHOD, "xml" );


				DOMSource src = new DOMSource( doc );

				StreamResult output = new StreamResult( new File( strFilename ) );

				transformer.transform( src, output );
			}
			catch( Exception e ){
				e.printStackTrace();
			}
		}
	}





	/**********************************************************************
	XPathとは、XML文章の中の要素や属性の位置を指定するための言語。
	XML文章をツリーとしてモデル化し、各ノード(要素や属性)への位置を指定できるようにします。
	また、指定したノードに対して、条件判定を行ったり、文字列計算をしたり、計算結果を使用したりすることも可能。

	●データモデル
		XPathは、XMLのツリー構造をモデル化し、その中で属性や要素の位置を特定特定します。
		例えば、以下のようなXMLのツリー構造をモデル化し、その中で属性や要素を特定します。

		このツリーを、XPathは以下のコードに分類し管理します。
			・ルーとノード
			・要素ノード
			・属性ノード

		<members>
			<member num="1">
				<name>ooooo pppppp</name>
				<addr>qqqqqqqqqqqqqqqqq</addr>
				<contact>
					<tel>xxx-xxxx-xxxxx</tel>
					<e_mail>yyyyyy@zzzzz.jp</e_mail>
				</contact>
			</member>
		</members>

		nameを特定したい場合、"members/member/name"というような形で特定します。
		これは「ルートノード＋要素ノード＋要素ノード」で表現されています。
		num属性を特定した場合、"members/member/@num"となります。

		XPathで使用するノードには以下の種類があります。

		[	種類		]	[	内容	]
		ルートノード		ツリーの一番上を示す
		要素ノード			要素を意味する
		属性ノード			属性を意味する
		テキストノード		要素や属性の値を意味する
		コメントノード		コメントを意味する
		処理命令ノード		処理命令を意味する
		名前空間ノード		名前空間を意味する


	●ロケーションパス
		XPathの表記は、「ロケーションパス」と「軸とノードテスト」で構成されてます。
		ロケーションパスはUNIXやURLと似たような表記方法で、"/"を区切りとしてノードパスを示していきます。
		ロケーションパスでノードを区切り、軸とノードテストでそのノードが何が何かを示します。

		ロケーションパスは、絶対表記と相対表記があり、
		絶対表記はルートノードから順に記述し、相対表記は、カレントからの位置関係を示します。

		<NodeA>
			<NodeB>
				<NodeC></NodeC>
			</NodeB>
		</NodeA>

		「絶対表記」
		：NodeA		"/NodeA"
		：NodeB		"/NodeA/NodeB"
		：NodeC		"/NodeA/NodeB/NodeC"

		「相対表記（カレントNodeB）」
		：NodeA		"../"
		：NodeB		"./"
		：NodeC		"./NodeC"
		
	●軸とノードテスト
		軸は、ノードの方向を示し、
		例えば軸が"self"なら自分自身の階層を指し、軸が"parent"なら自分より一つ上の階層を意味します。
		ノードテストは、そのノードの意味を示し、例えばコメントノードであれば、"comment()"で表現します。

		[Syntax]	軸：：ノードテスト


		軸の一覧
		self						基点ノード
		child						基点ノードの子
		parent						基点ノードの親
		ancestor					基点ノードの先祖(親も含む)
		ancestor-or-self			基点ノードの祖先(自分も含む)
		follwing-sibling			基点ノードの兄弟で基点ノードの後で出てくる
		preceding-sibling			基点ノードの兄弟で基点ノードの前に出てくる
		following					基点ノードの後で出てくる
		preceding					基点ノードの前で出てくる
		attribute					基点ノードの属性
		descendant-or-self			基点ノードの子孫 
		namespace					基点ノードの名前空間 


		ノードテストの一覧
		名前						要素や属性ノードを示す
		text()						テキストノードを示す
		node()						要素や属性を明示的に指す
		*							軸の子ノード全て
		comment()					コメントノードを指す
		processing-instruction()	処理命令ノードを指す

		

	 *
	 */
	class UsageXPath {

		/**********************************************************************
		 *
		 */
		public UsageXPath(){
			try{
				DocumentBuilder builder = DocumentBuilderFactory.newInstance().newDocumentBuilder();
				Document doc = builder.parse( new File("file.xml") );

				// ===== javax.xml.xpath.XPath を使う方法 =====
				// 準備
				XPathFactory factory = XPathFactory.newInstance();
				XPath xpath = factory.newXPath();

				// 単一ノード取得
				String location = "/test/t1/text/text()";
				System.out.println( xpath.evaluate(location, doc) );

				// 複数ノード取得
				location = "//t1/t2[2]/text()";
				NodeList entries = (NodeList) xpath.evaluate( location, doc, XPathConstants.NODESET );
				for( int i=0; i<entries.getLength(); i++ ) {
				    System.out.println( entries.item(i).getNodeValue() );
				}
			}
			catch( Exception e ){
				e.printStackTrace();
			}
		}
	}

	/**********************************************************************
	 *
	 */
	public static void		main( String[] args ){
		new UsageDOM();
	}	
}