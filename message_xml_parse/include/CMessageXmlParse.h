/*
 * CMessageXmlParse.h
 *
 *  Created on: 2011-10-21
 *      Author: zhangpeng
 */

#ifndef CMESSAGEXMLPARSE_H_
#define CMESSAGEXMLPARSE_H_
#include <string>
#include <map>
#include "ctinyxml.h"
#include "ctinystr.h"

class CXmlParse
{
public:
	CXmlParse();
	CXmlParse(std::string xmlContent);
	virtual ~CXmlParse();

	void setContent(const std::string mesageContent);
	void loadFile(std::string filename);
	void saveFile(std::string filename);

	bool getXml(std::string &messageXml);

protected:
	TiXmlDocument* pDoc;

	std::string strVersion;
	std::string strStandalone;
	std::string strEncoding;

	bool getXmlDeclare();
	bool getNodePointerByName(std::string &strNodeName, TiXmlElement* &Node);
	bool getNodePointerByName(TiXmlElement *pRootEle, std::string &strNodeName,
			TiXmlElement* &Node);
	bool queryNodeText(std::string strNodeName, std::string &strText);
	bool queryNodeAttribute(std::string strNodeName,
			std::map<std::string, std::string> &AttMap);
	bool delNode(std::string strNodeName);
};

class CMessageXmlParse: public CXmlParse
{
public:
	CMessageXmlParse();
	CMessageXmlParse(std::string xmlContent);
	virtual ~CMessageXmlParse();

	bool getResult(std::string &result);
	bool getConfId(std::string &ConfId);


	std::string createMessageXml(std::string targetId, std::string type, std::string src);
	std::string createConfCreateXml();
	std::string createConfJoinXml(std::string& connId, std::string& confId);
	std::string createConfModifyXml(std::string confId, int partiesNum);

	std::string createFastUpdateXml();

private:
	std::string create3PartiesXml(std::string confId);
	std::string create4PartiesXml(std::string confId);
	std::string create5PartiesXml(std::string confId);
	std::string create6PartiesXml(std::string confId);



};
#endif /* CMESSAGEXMLPARSE_H_ */
