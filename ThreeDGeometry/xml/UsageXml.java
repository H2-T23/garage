// UsageXml.java 

/***
�uW3C DOM 3 XPath�̊����ɏ]�������@��XPath��]������v
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
		[Java��XML������ǂݍ����DOM�c���[���\�z����T���v��]
		XML�������c���[�`���ň������߂̃��f���Ƃ���DOM������AW3D�����̎d�l�����J���Ă��܂��B
		�Ⴆ�΁ADOM 3 Core Specification �Ȃǂł��B

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
	XPath�Ƃ́AXML���͂̒��̗v�f�⑮���̈ʒu���w�肷�邽�߂̌���B
	XML���͂��c���[�Ƃ��ă��f�������A�e�m�[�h(�v�f�⑮��)�ւ̈ʒu���w��ł���悤�ɂ��܂��B
	�܂��A�w�肵���m�[�h�ɑ΂��āA����������s������A������v�Z��������A�v�Z���ʂ��g�p�����肷�邱�Ƃ��\�B

	���f�[�^���f��
		XPath�́AXML�̃c���[�\�������f�������A���̒��ő�����v�f�̈ʒu�������肵�܂��B
		�Ⴆ�΁A�ȉ��̂悤��XML�̃c���[�\�������f�������A���̒��ő�����v�f����肵�܂��B

		���̃c���[���AXPath�͈ȉ��̃R�[�h�ɕ��ނ��Ǘ����܂��B
			�E���[�ƃm�[�h
			�E�v�f�m�[�h
			�E�����m�[�h

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

		name����肵�����ꍇ�A"members/member/name"�Ƃ����悤�Ȍ`�œ��肵�܂��B
		����́u���[�g�m�[�h�{�v�f�m�[�h�{�v�f�m�[�h�v�ŕ\������Ă��܂��B
		num��������肵���ꍇ�A"members/member/@num"�ƂȂ�܂��B

		XPath�Ŏg�p����m�[�h�ɂ͈ȉ��̎�ނ�����܂��B

		[	���		]	[	���e	]
		���[�g�m�[�h		�c���[�̈�ԏ������
		�v�f�m�[�h			�v�f���Ӗ�����
		�����m�[�h			�������Ӗ�����
		�e�L�X�g�m�[�h		�v�f�⑮���̒l���Ӗ�����
		�R�����g�m�[�h		�R�����g���Ӗ�����
		�������߃m�[�h		�������߂��Ӗ�����
		���O��ԃm�[�h		���O��Ԃ��Ӗ�����


	�����P�[�V�����p�X
		XPath�̕\�L�́A�u���P�[�V�����p�X�v�Ɓu���ƃm�[�h�e�X�g�v�ō\������Ă܂��B
		���P�[�V�����p�X��UNIX��URL�Ǝ����悤�ȕ\�L���@�ŁA"/"����؂�Ƃ��ăm�[�h�p�X�������Ă����܂��B
		���P�[�V�����p�X�Ńm�[�h����؂�A���ƃm�[�h�e�X�g�ł��̃m�[�h�����������������܂��B

		���P�[�V�����p�X�́A��Ε\�L�Ƒ��Ε\�L������A
		��Ε\�L�̓��[�g�m�[�h���珇�ɋL�q���A���Ε\�L�́A�J�����g����̈ʒu�֌W�������܂��B

		<NodeA>
			<NodeB>
				<NodeC></NodeC>
			</NodeB>
		</NodeA>

		�u��Ε\�L�v
		�FNodeA		"/NodeA"
		�FNodeB		"/NodeA/NodeB"
		�FNodeC		"/NodeA/NodeB/NodeC"

		�u���Ε\�L�i�J�����gNodeB�j�v
		�FNodeA		"../"
		�FNodeB		"./"
		�FNodeC		"./NodeC"
		
	�����ƃm�[�h�e�X�g
		���́A�m�[�h�̕����������A
		�Ⴆ�Ύ���"self"�Ȃ玩�����g�̊K�w���w���A����"parent"�Ȃ玩�������̊K�w���Ӗ����܂��B
		�m�[�h�e�X�g�́A���̃m�[�h�̈Ӗ��������A�Ⴆ�΃R�����g�m�[�h�ł���΁A"comment()"�ŕ\�����܂��B

		[Syntax]	���F�F�m�[�h�e�X�g


		���̈ꗗ
		self						��_�m�[�h
		child						��_�m�[�h�̎q
		parent						��_�m�[�h�̐e
		ancestor					��_�m�[�h�̐�c(�e���܂�)
		ancestor-or-self			��_�m�[�h�̑c��(�������܂�)
		follwing-sibling			��_�m�[�h�̌Z��Ŋ�_�m�[�h�̌�ŏo�Ă���
		preceding-sibling			��_�m�[�h�̌Z��Ŋ�_�m�[�h�̑O�ɏo�Ă���
		following					��_�m�[�h�̌�ŏo�Ă���
		preceding					��_�m�[�h�̑O�ŏo�Ă���
		attribute					��_�m�[�h�̑���
		descendant-or-self			��_�m�[�h�̎q�� 
		namespace					��_�m�[�h�̖��O��� 


		�m�[�h�e�X�g�̈ꗗ
		���O						�v�f�⑮���m�[�h������
		text()						�e�L�X�g�m�[�h������
		node()						�v�f�⑮���𖾎��I�Ɏw��
		*							���̎q�m�[�h�S��
		comment()					�R�����g�m�[�h���w��
		processing-instruction()	�������߃m�[�h���w��

		

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

				// ===== javax.xml.xpath.XPath ���g�����@ =====
				// ����
				XPathFactory factory = XPathFactory.newInstance();
				XPath xpath = factory.newXPath();

				// �P��m�[�h�擾
				String location = "/test/t1/text/text()";
				System.out.println( xpath.evaluate(location, doc) );

				// �����m�[�h�擾
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