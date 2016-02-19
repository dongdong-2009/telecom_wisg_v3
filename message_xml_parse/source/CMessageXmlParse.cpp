/*
 * CMessageXmlParse.C
 *
 *  Created on: 2011-10-21
 *      Author: zhangpeng
 */
#include "CMessageXmlParse.h"
using namespace std;

CXmlParse::CXmlParse() {
	this->pDoc = NULL;
	this->setContent("");
}

CXmlParse::CXmlParse(string xmlContent) {
	this->pDoc = NULL;
	this->setContent(xmlContent);
}

CXmlParse::~CXmlParse() {
	if (this->pDoc != NULL) {
		//delete this->pDoc;
		pDoc = NULL;
	}
}

void CXmlParse::setContent(string xmlContent) {
	if (pDoc != NULL) {
		//delete pDoc;
		pDoc = NULL;
	}

	this->pDoc = new TiXmlDocument;
	if (NULL == this->pDoc) {
		exit(1);
	}
	this->pDoc->Parse(xmlContent.c_str());
	this->getXmlDeclare();
}

void CXmlParse::loadFile(string filename) {
	if (pDoc != NULL) {
		//delete pDoc;
		pDoc = NULL;
	}

	this->pDoc = new TiXmlDocument;
	if (NULL == this->pDoc) {
		exit(1);
	}
	this->pDoc->LoadFile(filename);
	this->getXmlDeclare();
}

void CXmlParse::saveFile(string filename) {
	this->pDoc->SaveFile(filename);
}

bool CXmlParse::getXmlDeclare() {
	TiXmlNode* pXmlFirst = pDoc->FirstChild();
	if (NULL != pXmlFirst) {
		TiXmlDeclaration *pXmlDec = pXmlFirst->ToDeclaration();
		if (NULL != pXmlDec) {
			strVersion = pXmlDec->Version();
			strStandalone = pXmlDec->Standalone();
			strEncoding = pXmlDec->Encoding();
			return true;
		}
	}
	return false;
}

bool CXmlParse::getXml(string &messageXml) {
	messageXml << *(this->pDoc);
	return true;
}

bool CXmlParse::getNodePointerByName(std::string &strNodeName,
		TiXmlElement* &Node) {
	TiXmlElement *root = pDoc->RootElement();
	return this->getNodePointerByName(root, strNodeName, Node);
}

bool CXmlParse::getNodePointerByName(TiXmlElement *pRootEle,
		std::string &strNodeName, TiXmlElement* &Node) {
	if (strNodeName == pRootEle->Value()) {
		Node = pRootEle;
		return true;
	}
	TiXmlElement* pEle = pRootEle->FirstChildElement();
	for (; pEle; pEle = pEle->NextSiblingElement()) {
		if (getNodePointerByName(pEle, strNodeName, Node))
			return true;
	}
	return false;
}

bool CXmlParse::queryNodeText(std::string strNodeName, std::string &strText) {
	TiXmlElement *pRootEle = pDoc->RootElement();
	if (NULL == pRootEle) {
		return false;
	}

	TiXmlElement *pNode = NULL;
	getNodePointerByName(pRootEle, strNodeName, pNode);
	if (NULL != pNode) {
		const char* psz = pNode->GetText();
		if (NULL == psz) {
			strText = "";
		} else {
			strText = psz;
		}
		return true;
	} else {
		return false;
	}
}

bool CXmlParse::queryNodeAttribute(std::string strNodeName, std::map<
		std::string, std::string> &AttMap) {
	typedef pair<string, string> String_Pair;
	TiXmlElement*pRootEle = pDoc->RootElement();
	if (NULL == pRootEle) {
		return false;
	}
	TiXmlElement *pNode = NULL;
	getNodePointerByName(pRootEle, strNodeName, pNode);
	if (NULL != pNode) {
		TiXmlAttribute* pAttr = NULL;
		string strAttrName;
		string strAttrValue;
		for (pAttr = pNode->FirstAttribute(); pAttr; pAttr = pAttr->Next()) {
			strAttrName = pAttr->Name();
			strAttrValue = pAttr->Value();
			AttMap.insert(String_Pair(strAttrName, strAttrValue));
		}
		return true;
	} else {
		return false;
	}
	return true;
}

bool CXmlParse::delNode(string strNodeName) {
	TiXmlElement *pRootEle = pDoc->RootElement();
	if (NULL == pRootEle) {
		return false;
	}
	TiXmlElement *pNode = NULL;
	getNodePointerByName(pRootEle, strNodeName, pNode);

	if (pRootEle == pNode) {
		if (pDoc->RemoveChild(pRootEle)) {
			return true;
		} else
			return false;
	}

	if (NULL != pNode) {
		TiXmlNode *pParNode = pNode->Parent();
		if (NULL == pParNode) {
			return false;
		}

		TiXmlElement* pParentEle = pParNode->ToElement();
		if (NULL != pParentEle) {
			if (pParentEle->RemoveChild(pNode))
				return true;
			else
				return false;
		}
	} else {
		return false;
	}
	return false;
}

CMessageXmlParse::CMessageXmlParse() :
	CXmlParse() {

}

CMessageXmlParse::CMessageXmlParse(string xmlContent) :
	CXmlParse(xmlContent) {

}

CMessageXmlParse::~CMessageXmlParse() {

}

bool CMessageXmlParse::getResult(string &result) {
	map<string, string> attr;
	if (this->queryNodeAttribute("result", attr)) {

		if (attr.find("response") != attr.end()) {
			result = attr["response"];
			printf("result %s\n", result.c_str());
			return true;
		}
	} else {
		return false;
	}

	return false;
}

bool CMessageXmlParse::getConfId(string &confId) {
	if (this->queryNodeText("confid", confId)) {
		return true;
	}
	return false;
}

//bool CMessageXmlParse::transMessage(string &message)
//{
//	//TODO:changed here
//	string sendTime,messageContent;
//	bool ret = this->getSendTime(sendTime);
//	ret = ret && this->getMessage(messageContent);
//	message = this->createMessageXml(sendTime,messageContent);
//
//	return ret;
//	//TODO:changed end;
//
//	/*bool*/ ret = this->delNode("DestionContactInfo");
//	getXml(message);
//	return ret;
//}

string CMessageXmlParse::createMessageXml(string targetId, string type,
		string src) {

	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pDialog = new TiXmlElement("dialogstart");
	if (NULL == pDialog) {
		exit(1);
	}
	targetId = "conn:" + targetId;
	pDialog->SetAttribute("target", targetId.c_str());
	pDialog->SetAttribute("type", type.c_str());
	pDialog->SetAttribute("src", src.c_str());
	pRootEle->LinkEndChild(pDialog);

	string ret;
	tDoc->Print();
	ret << *(tDoc);

	//delete tDoc;
	return ret;
}

string CMessageXmlParse::createConfCreateXml() {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pCreate = new TiXmlElement("createconference");
	if (NULL == pCreate) {
		exit(1);
	}

	pRootEle->LinkEndChild(pCreate);

	TiXmlElement *pAudioMix = new TiXmlElement("audiomix");
	if (NULL == pAudioMix) {
		exit(1);
	}
	pCreate->LinkEndChild(pAudioMix);

	TiXmlElement *pAsn = new TiXmlElement("asn");
	if (NULL == pAsn) {
		exit(1);
	}
	pAsn->SetAttribute("ri", "00s");
	pAudioMix->LinkEndChild(pAsn);

	TiXmlElement *pLayout = new TiXmlElement("videolayout");
	if (NULL == pLayout) {
		exit(1);
	}
	pCreate->LinkEndChild(pLayout);

	TiXmlElement *pRoot = new TiXmlElement("root");
	if (NULL == pRoot) {
		exit(1);
	}
	pRoot->SetAttribute("size", "VGA");
	pLayout->LinkEndChild(pRoot);

	//	TiXmlElement *pSelector = new TiXmlElement("selector");
	//	pSelector->SetAttribute("id", "switch");
	//	pSelector->SetAttribute("method", "vas");
	//	pLayout->LinkEndChild(pSelector);


	TiXmlElement *pRegion1 = new TiXmlElement("region");
	if (NULL == pRegion1) {
		exit(1);
	}
	pRegion1->SetAttribute("id", "1");
	pRegion1->SetAttribute("left", "25%");
	pRegion1->SetAttribute("top", "0");
	pRegion1->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion1);

	TiXmlElement *pRegion2 = new TiXmlElement("region");
	if (NULL == pRegion2) {
		exit(1);
	}
	pRegion2->SetAttribute("id", "2");
	pRegion2->SetAttribute("left", "0%");
	pRegion2->SetAttribute("top", "50%");
	pRegion2->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion2);

	TiXmlElement *pRegion3 = new TiXmlElement("region");
	if (NULL == pRegion3) {
		exit(1);
	}
	pRegion3->SetAttribute("id", "3");
	pRegion3->SetAttribute("left", "50%");
	pRegion3->SetAttribute("top", "50%");
	pRegion3->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion3);

	//	TiXmlElement *pRegion4 = new TiXmlElement("region");
	//	if(NULL == pRegion4){
	//		exit(1);
	//	}
	//	pRegion4->SetAttribute("id", "4");
	//	pRegion4->SetAttribute("left", "50%");
	//	pRegion4->SetAttribute("top", "50%");
	//	pRegion4->SetAttribute("relativesize", "1/2");
	//	pLayout->LinkEndChild(pRegion4);

	string ret;
	ret << *(tDoc);

	delete tDoc;
	return ret;
}

string CMessageXmlParse::createConfJoinXml(string& connId, string& confId) {
	string res = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><msml version=\"1.1\"><join id1=\"";
	res += connId;
	res += "\" id2=\"";
	res += confId;
	res += "\"><stream media=\"audio\" /><stream media=\"video\" dir=\"from-id1\" display=\"switch\" /><stream media=\"video\" dir=\"to-id1\" /></join></msml>";
	return res;

//		TiXmlDocument* tDoc = new TiXmlDocument;
//		if(NULL == tDoc)
//		{
//			exit(1);
//		}
//		TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","UTF-8","no");
//		if(NULL == pDeclaration)
//		{
//			exit(1);
//		}
//		tDoc->LinkEndChild(pDeclaration);
//		printf("0\n");
//		TiXmlElement *pRootEle = new TiXmlElement("msml");
//		if(NULL == pRootEle)
//		{
//			exit(1);
//		}
//		pRootEle->SetAttribute("version", "1.1");
//		printf("1\n");
//		tDoc->LinkEndChild(pRootEle);
//
//		TiXmlElement * pJoin = new TiXmlElement("join");
//		printf("%x, %x\n", pJoin, this);
//		if(pJoin == NULL){
//			exit(1);
//		}
//		printf("-1");
//		printf("Id:: %s %s\n", connId.c_str(), confId.c_str());
//		pJoin->SetAttribute("id1", connId);
//		printf("2\n");
//		pJoin->SetAttribute("id2", confId);
//		printf("3\n");
//		pRootEle->LinkEndChild(pJoin);
//
//		TiXmlElement * pStream1 = new TiXmlElement("stream");
//		pStream1->SetAttribute("media", "audio");
//		pJoin->LinkEndChild(pStream1);
//
//		TiXmlElement * pStream2 = new TiXmlElement("stream");
//		pStream2->SetAttribute("media", "video");
//		pStream2->SetAttribute("dir", "from-id1");
//		pStream2->SetAttribute("display", "switch");
//		pJoin->LinkEndChild(pStream2);
//
//		TiXmlElement * pStream3 = new TiXmlElement("stream");
//		pStream3->SetAttribute("media", "video");
//		pStream3->SetAttribute("dir", "to-id1");
//		pJoin->LinkEndChild(pStream3);
//
//		string ret;
//	//	tDoc->Print();
//		ret<<*(tDoc);
//
//		delete tDoc;
//		return ret;
}

string CMessageXmlParse::createConfModifyXml(string confId, int partiesNum) {

	string str;
	if (partiesNum == 3) {
		str = create3PartiesXml(confId);
	} else if (partiesNum == 4) {
		str = create4PartiesXml(confId);
	} else if (partiesNum == 5) {
		str = create5PartiesXml(confId);
	} else if (partiesNum == 6) {
		str = create6PartiesXml(confId);
	} else {
		perror("invalid party num");
	}
	return str;

}

string CMessageXmlParse::create3PartiesXml(string confId) {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pCreate = new TiXmlElement("modifyconference");
	if (NULL == pCreate) {
		exit(1);
	}
	pCreate->SetAttribute("id", confId.c_str());
	pRootEle->LinkEndChild(pCreate);

	TiXmlElement *pLayout = new TiXmlElement("videolayout");
	if (NULL == pLayout) {
		exit(1);
	}
	pCreate->LinkEndChild(pLayout);

	TiXmlElement *pRoot = new TiXmlElement("root");
	if (NULL == pRoot) {
		exit(1);
	}
	pRoot->SetAttribute("size", "VGA");
	pLayout->LinkEndChild(pRoot);

	//	TiXmlElement *pSelector = new TiXmlElement("selector");
	//	pSelector->SetAttribute("id", "switch");
	//	pSelector->SetAttribute("method", "vas");
	//	pLayout->LinkEndChild(pSelector);


	TiXmlElement *pRegion1 = new TiXmlElement("region");
	if (NULL == pRegion1) {
		exit(1);
	}
	pRegion1->SetAttribute("id", "1");
	pRegion1->SetAttribute("left", "25%");
	pRegion1->SetAttribute("top", "0");
	pRegion1->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion1);

	TiXmlElement *pRegion2 = new TiXmlElement("region");
	if (NULL == pRegion2) {
		exit(1);
	}
	pRegion2->SetAttribute("id", "2");
	pRegion2->SetAttribute("left", "0");
	pRegion2->SetAttribute("top", "50%");
	pRegion2->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion2);

	TiXmlElement *pRegion3 = new TiXmlElement("region");
	if (NULL == pRegion3) {
		exit(1);
	}
	pRegion3->SetAttribute("id", "3");
	pRegion3->SetAttribute("left", "50%");
	pRegion3->SetAttribute("top", "50%");
	pRegion3->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion3);

	string ret;
	ret << *(tDoc);

	delete tDoc;
	return ret;
}

string CMessageXmlParse::create4PartiesXml(string confId) {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pCreate = new TiXmlElement("modifyconference");
	if (NULL == pCreate) {
		exit(1);
	}
	pCreate->SetAttribute("id", confId.c_str());
	pRootEle->LinkEndChild(pCreate);

	TiXmlElement *pLayout = new TiXmlElement("videolayout");
	if (NULL == pLayout) {
		exit(1);
	}
	pCreate->LinkEndChild(pLayout);

	TiXmlElement *pRoot = new TiXmlElement("root");
	if (NULL == pRoot) {
		exit(1);
	}
	pRoot->SetAttribute("size", "VGA");
	pLayout->LinkEndChild(pRoot);

	//	TiXmlElement *pSelector = new TiXmlElement("selector");
	//	pSelector->SetAttribute("id", "switch");
	//	pSelector->SetAttribute("method", "vas");
	//	pLayout->LinkEndChild(pSelector);


	TiXmlElement *pRegion1 = new TiXmlElement("region");
	if (NULL == pRegion1) {
		exit(1);
	}
	pRegion1->SetAttribute("id", "1");
	pRegion1->SetAttribute("left", "0");
	pRegion1->SetAttribute("top", "0");
	pRegion1->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion1);

	TiXmlElement *pRegion2 = new TiXmlElement("region");
	if (NULL == pRegion2) {
		exit(1);
	}
	pRegion2->SetAttribute("id", "2");
	pRegion2->SetAttribute("left", "50%");
	pRegion2->SetAttribute("top", "0");
	pRegion2->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion2);

	TiXmlElement *pRegion3 = new TiXmlElement("region");
	if (NULL == pRegion3) {
		exit(1);
	}
	pRegion3->SetAttribute("id", "3");
	pRegion3->SetAttribute("left", "0");
	pRegion3->SetAttribute("top", "50%");
	pRegion3->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion3);

	TiXmlElement *pRegion4 = new TiXmlElement("region");
	if (NULL == pRegion4) {
		exit(1);
	}
	pRegion4->SetAttribute("id", "4");
	pRegion4->SetAttribute("left", "50%");
	pRegion4->SetAttribute("top", "50%");
	pRegion4->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion4);

	string ret;
	ret << *(tDoc);

	delete tDoc;
	return ret;
}

string CMessageXmlParse::create5PartiesXml(string confId) {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pCreate = new TiXmlElement("modifyconference");
	if (NULL == pCreate) {
		exit(1);
	}
	pCreate->SetAttribute("id", confId.c_str());
	pRootEle->LinkEndChild(pCreate);

	TiXmlElement *pLayout = new TiXmlElement("videolayout");
	if (NULL == pLayout) {
		exit(1);
	}
	pCreate->LinkEndChild(pLayout);

	TiXmlElement *pRoot = new TiXmlElement("root");
	if (NULL == pRoot) {
		exit(1);
	}
	pRoot->SetAttribute("size", "VGA");
	pLayout->LinkEndChild(pRoot);

	TiXmlElement *pRegion1 = new TiXmlElement("region");
	if (NULL == pRegion1) {
		exit(1);
	}
	pRegion1->SetAttribute("id", "1");
	pRegion1->SetAttribute("left", "0");
	pRegion1->SetAttribute("top", "0");
	pRegion1->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion1);

	TiXmlElement *pRegion2 = new TiXmlElement("region");
	if (NULL == pRegion2) {
		exit(1);
	}
	pRegion2->SetAttribute("id", "2");
	pRegion2->SetAttribute("left", "50%");
	pRegion2->SetAttribute("top", "0");
	pRegion2->SetAttribute("relativesize", "1/2");
	pLayout->LinkEndChild(pRegion2);

	TiXmlElement *pRegion3 = new TiXmlElement("region");
	if (NULL == pRegion3) {
		exit(1);
	}
	pRegion3->SetAttribute("id", "3");
	pRegion3->SetAttribute("left", "0");
	pRegion3->SetAttribute("top", "50%");
	pRegion3->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion3);

	TiXmlElement *pRegion4 = new TiXmlElement("region");
	if (NULL == pRegion4) {
		exit(1);
	}
	pRegion4->SetAttribute("id", "4");
	pRegion4->SetAttribute("left", "33%");
	pRegion4->SetAttribute("top", "50%");
	pRegion4->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion4);

	TiXmlElement *pRegion5 = new TiXmlElement("region");
	if (NULL == pRegion5) {
		exit(1);
	}
	pRegion5->SetAttribute("id", "5");
	pRegion5->SetAttribute("left", "66%");
	pRegion5->SetAttribute("top", "50%");
	pRegion5->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion5);

	string ret;
	ret << *(tDoc);

	delete tDoc;
	return ret;

}

string CMessageXmlParse::create6PartiesXml(string confId) {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement *pRootEle = new TiXmlElement("msml");
	if (NULL == pRootEle) {
		exit(1);
	}
	pRootEle->SetAttribute("version", "1.1");
	tDoc->LinkEndChild(pRootEle);

	TiXmlElement *pCreate = new TiXmlElement("modifyconference");
	if (NULL == pCreate) {
		exit(1);
	}
	pCreate->SetAttribute("id", confId.c_str());
	pRootEle->LinkEndChild(pCreate);

	TiXmlElement *pLayout = new TiXmlElement("videolayout");
	if (NULL == pLayout) {
		exit(1);
	}
	pCreate->LinkEndChild(pLayout);

	TiXmlElement *pRoot = new TiXmlElement("root");
	if (NULL == pRoot) {
		exit(1);
	}
	pRoot->SetAttribute("size", "VGA");
	pLayout->LinkEndChild(pRoot);

	TiXmlElement *pRegion1 = new TiXmlElement("region");
	if (NULL == pRegion1) {
		exit(1);
	}
	pRegion1->SetAttribute("id", "1");
	pRegion1->SetAttribute("left", "0");
	pRegion1->SetAttribute("top", "0");
	pRegion1->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion1);

	TiXmlElement *pRegion2 = new TiXmlElement("region");
	if (NULL == pRegion2) {
		exit(1);
	}
	pRegion2->SetAttribute("id", "2");
	pRegion2->SetAttribute("left", "33%");
	pRegion2->SetAttribute("top", "0");
	pRegion2->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion2);

	TiXmlElement *pRegion3 = new TiXmlElement("region");
	if (NULL == pRegion3) {
		exit(1);
	}

	pRegion3->SetAttribute("id", "3");
	pRegion3->SetAttribute("left", "66%");
	pRegion3->SetAttribute("top", "0");
	pRegion3->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion3);

	TiXmlElement *pRegion4 = new TiXmlElement("region");
	if (NULL == pRegion4) {
		exit(1);
	}

	pRegion4->SetAttribute("id", "4");
	pRegion4->SetAttribute("left", "0");
	pRegion4->SetAttribute("top", "50%");
	pRegion4->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion4);

	TiXmlElement *pRegion5 = new TiXmlElement("region");
	if (NULL == pRegion5) {
		exit(1);
	}
	pRegion5->SetAttribute("id", "5");
	pRegion5->SetAttribute("left", "33%");
	pRegion5->SetAttribute("top", "50%");
	pRegion5->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion5);

	TiXmlElement *pRegion6 = new TiXmlElement("region");
	if (NULL == pRegion6) {
		exit(1);
	}
	pRegion6->SetAttribute("id", "6");
	pRegion6->SetAttribute("left", "66%");
	pRegion6->SetAttribute("top", "50%");
	pRegion6->SetAttribute("relativesize", "1/3");
	pLayout->LinkEndChild(pRegion6);

	string ret;
	ret << *(tDoc);

	delete tDoc;
	return ret;

	return string();
}

string CMessageXmlParse::createFastUpdateXml() {
	TiXmlDocument* tDoc = new TiXmlDocument;
	if (NULL == tDoc) {
		exit(1);
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0", "UTF-8", "no");
	if (NULL == pDeclaration) {
		exit(1);
	}
	tDoc->LinkEndChild(pDeclaration);

	TiXmlElement * pMedia = new TiXmlElement("media_control");
	tDoc->LinkEndChild(pMedia);

	TiXmlElement * pVc = new TiXmlElement("vc_primitive");

	pMedia->LinkEndChild(pVc);

	TiXmlElement * pTo = new TiXmlElement("to_encoder");
	pVc->LinkEndChild(pTo);

	TiXmlElement * pPic = new TiXmlElement("picture_fast_update");
	pTo->LinkEndChild(pPic);

	string ret;
	//	tDoc->Print();
	ret << *(tDoc);

	delete tDoc;
	return ret;
}

//string CMessageXmlParse::createMessageXml(string status,string host,string port,string sendTime,string content)
//{
//	//TODO:changed here
//	string message1 = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?><DICE_MESSAGE><DestionContactInfo><Status>";
//
//	string message2 = "</Status><RealAddress>";
//
//	string message3 = "</RealAddress><RealPort>";
//
//	string message4 = "</RealPort></DestionContactInfo><Message> <SendTime>";
//
//	string message5 = "</SendTime><MessageContent>";
//
//	string message6 = "</MessageContent></Message></DICE_MESSAGE>";
//
//	string messageXml = message1 + status + message2 + host + message3 + port + message4 + sendTime + message5 + content + message6;
//
//	return messageXml;
//	//TODO:changed end;
//
//}
