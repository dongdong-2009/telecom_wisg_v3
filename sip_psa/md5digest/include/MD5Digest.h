/**
 * @file HttpDigest.h
 * @Synopsis
 * @author yushun
 * @date 2009-03-03
 */


#ifndef  _HttpDigest_H
#define  _HttpDigest_H

#include <string>
#include "md5.h"
#include "str.h"

#include "DigestAKA.h"
using std::string;



class MD5Digest{
    private:
        string      password;
        string      username;
        string      realm;
        string      nonce;
        string      cnonce;
        string      noncecount;
        string      qop;
        string      method;
        string      uri;
        string      response;
        string      algorithm;

        string 		hash1;
        string 		hash2;

        int aka_version;

    public:

        MD5Digest(string &password,string &username,string &realm, string &nonce,
                string &cnonce,string &noncecount,string algorithm, string qop="auth",
                string method="REGISTER"):
            password(password),username(username),realm(realm),nonce(nonce),cnonce(cnonce),
            noncecount(noncecount),qop(qop),method(method),algorithm(algorithm)
        {
           uri = string("sip:")+realm;
        }

       string calcResponse();

    private:
       string calcHASH1(string username, string realm, string password);
       string calcHASH2(string method, string uri);
       string calcResult(string hash1, string nonce, string nc, string cnonce, string qop, string hash2);
};

#endif
