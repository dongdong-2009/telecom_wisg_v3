#include "MD5Digest.h"

string MD5Digest::calcResponse()
{

	if(algorithm.compare("AKAv1-MD5") == 0)
		aka_version = 1;
	else if(algorithm.compare("AKAv2-MD5") == 0)
		aka_version = 2;
	else if(algorithm.compare("MD5") == 0)
		aka_version = 0;
	else{
		fprintf(stderr, "Authentication scheme %s not supported\n", algorithm.c_str());
		return NULL;
	}


	if(aka_version > 0){
		str nonceStr, k;
		nonceStr.s = (char *) nonce.c_str();
		nonceStr.len = nonce.length();
		k.s = (char *)password.c_str();
		k.len = password.length();
		str res = AKA(aka_version,nonceStr, k);
		password = res.s;
	}



	this->hash1 = calcHASH1(username,realm,password);
	this->hash2 = calcHASH2(method, uri);

	string response = calcResult(hash1,nonce,noncecount,cnonce,qop,hash2);
	return response;
}

string MD5Digest::calcHASH1(string username, string realm, string password)
{
	MD5 md5;
	md5.init();
	md5.UpdateMd5(username.c_str(), username.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(realm.c_str(), realm.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(password.c_str(), password.length());
	md5.Finalize();

	return md5.GetMd5();
}

string MD5Digest::calcHASH2(string method, string uri)
{
	MD5 md5;
	md5.init();
	md5.UpdateMd5(method.c_str(), method.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(uri.c_str(), uri.length());

	md5.Finalize();

	return md5.GetMd5();
}

string MD5Digest::calcResult(string hash1, string nonce, string nc, string cnonce, string qop, string hash2)
{
	MD5 md5;
	md5.init();
	md5.UpdateMd5(hash1.c_str(), hash1.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(nonce.c_str(), nonce.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(nc.c_str(), nc.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(cnonce.c_str(), cnonce.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(qop.c_str(), qop.length());
	md5.UpdateMd5(":", 1);
	md5.UpdateMd5(hash2.c_str(), hash2.length());
	md5.Finalize();

	return md5.GetMd5();

}
