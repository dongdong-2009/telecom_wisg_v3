#include <iostream>
#include <string>
#include "CMessageXmlParse.h"
using namespace std;

int main()
{
	CMessageXmlParse *parser = new CMessageXmlParse;
//	//parser->loadFile("test.xml");
//
//	string xml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\">"
//			"<DICE_MESSAGE>"
//			"<DestionContactInfo>"
//			"<Status>true</Status>"
//			"<RealAddress>59.64.179.211</RealAddress>"
//			"<RealPort>5060</RealPort>"
//			"</DestionContactInfo>"
//			"<Message>"
//			"<SendTime>2011-10-20 15:48:31</SendTime>"
//			"<MessageContent>hello</MessageContent>"
//			"</Message>"
//			"</DICE_MESSAGE>";
//	cout<<xml1<<endl;
//	parser->setContent(xml1);
//
//	string addr,port,message,sendTime,message_xml;

//	parser->getXml(message);
//	cout<<message<<endl;
//	parser->getHost(addr);
//	parser->getPort(port);
//	parser->getMessage(message);
//	parser->getSendTime(sendTime);
//	parser->getXml(message_xml);
//	cout<<addr<<" "<<port<<" "<<message<<" "<<sendTime<<" "<<endl;
//	cout<<message_xml<<endl;
//	bool status,ret;
//	ret = parser->getStatus(status);
//	if(status)cout<<"true"<<endl;
//	else cout<<"false"<<endl;
//	//string xml = parser->createMessageXml("f396e898-8ff76d0a-13c4-50022-131-502bf90c-131", "application/moml+xml",
//			"file:///var/lib/xms/media/en-US/msml/scripts/VideoMainMenu.moml");
//	//cout<<endl<<xml<<endl;
//
//	//parser->transMessage(message);
//	//cout<<message<<endl;
//	//parser->saveFile("message.xml");

	string s1 = "conn:f72802b0-93f76d0a-13c4-50022-330-139cd9b-330";
	string s2 = "conn:f7280108-93f76d0a-13c4-50022-330-6079e33f-330";

	string xml = parser->createConfJoinXml(s1, s2);

	cout<<xml<<endl;
	delete parser;
	parser = NULL;

	return 0;
}
