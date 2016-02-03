CC = g++
OUTPUT_DIR = .
DEPLOY_DIR = ./unidir
LIB_DIR= $(DEPLOY_DIR)/lib

D_SIPMSGDEF = ./sip_psa/msgdef
D_SRC_SIPMSGDEF = $(D_SIPMSGDEF)/source
D_PSASIP = ./sip_psa/psasip
D_SRC_PSASIP = $(D_PSASIP)/source
D_MSGLIB = ./sip_psa/encoder
D_SRC_MSGLIB = $(D_MSGLIB)/source
D_MD5DIGEST = ./sip_psa/md5digest
D_SRC_MD5DIGEST = $(D_MD5DIGEST)/source


INC_DIR=  -I./rtcpsa/include \
		  -I./utils/include \
		  -I./msgdef_rtc/include \
		  -I./msgdef_internal/include \
		  -I./dbhandler/include \
		  -I./dispatcher/include \
		  -I./bear/include \
		  -I./rtc_sip_callmodule/include \
		  -I./message_xml_parse/include \
	      -I./message_xml_parse/tinyxml/include \
	      -I/usr/local/include/ \
		  -I$(D_SIPMSGDEF)/include -I$(D_MD5DIGEST)/include -I$(D_PSASIP)/include -I$(D_MSGLIB)/include \
		  -I/usr/local/include/mcf
		  
CFLAGS = -Wall -g $(INC_DIR) -D_DEBUG  -Wno-deprecated -Wno-write-strings
#CFLAGS = -Wall -g $(INC_DIR)  -Wno-deprecated -Wno-write-strings

LIBS = -L. -L$(LIB_DIR) -lsipmsgdef -ljsoncpp -lmcf -llog4cxx -lutils -lmysqlclient -lrtcmsg -lrtcdb  -lintmsg\
		-losip2 -losipparser2 -leXosip2 -lpthread 

%.o: %.C
	$(CC) -c -o $@ $(CFLAGS) $<
#%.o : %.C
#	$(CXX) $(CXXFLAGS) -fPIC -c -o $@  $<
%.o : %.cpp
	$(CC) -c -o $@ $(CFLAGS) $<
#%.o : %.cpp
#	$(CC) $(CXXFLAGS) -fPIC -c -o $@ $<
#=========== USAGE ===================
usage:
	@echo "[Make Usage] :"
	@echo "    make all                             compile all"
	@echo "        make rtc_psa                 	compile rtcpsa"
	@echo "        make test_server                 compile server(test sockets and rtcpsa)" 
	@echo "        make rtc_msgdef                  compile msgdef"
	@echo "        make rtc_db                      compile dbhandler"
	@echo "    make clean                           clean all"  
	@echo "    make deploy                          deploy components to ./unidir/"
#	@echo "    make compilemsg                      compile msg"
#	@echo "    make compilesm                       compile SM"

all: sipmsgdef rtc_msgdef internal_msgdef rtc_db rtc_psa dispatcher \
	rtc_sip_callmodule psasip msglib bear_module
clean: clean.rtc_psa clean.rtc_msgdef clean.rtc_db clean.internal_msgdef clean.dispatcher clean.deploy clean.message_xml_parse \
	 clean.rtc_sip_callmodule clean.bear_module\
	clean.psasip clean.msglib clean.sipmsgdef
#compilesm: compilertcsm compilesipsm


#=========== sip_psa ================
OBJ_SIP_MSG = $(D_SRC_SIPMSGDEF)/msgdef_sip.o $(D_SRC_SIPMSGDEF)/msgdatadef_sip.o \
	$(D_SRC_SIPMSGDEF)/msghelper_sip.o $(D_SRC_SIPMSGDEF)/msgdatadef_sip.pb.o

OBJ_PSA_SIP = $(D_SRC_PSASIP)/exosipstack.o $(D_SRC_PSASIP)/exosiptranslator.o \
$(D_SRC_PSASIP)/hooksipimpl.o $(D_SRC_PSASIP)/psasip.o \
$(D_SRC_PSASIP)/sipenv.o  $(D_SRC_PSASIP)/CSipUserManager.o

OBJ_MD5DIGEST = $(D_SRC_MD5DIGEST)/base64.o $(D_SRC_MD5DIGEST)/rijndael.o \
	$(D_SRC_MD5DIGEST)/milenage.o $(D_SRC_MD5DIGEST)/DigestAKA.o \
	$(D_SRC_MD5DIGEST)/md5.o $(D_SRC_MD5DIGEST)/MD5Digest.o

OBJ_MSGLIB = $(D_SRC_MSGLIB)/sipmsglib.o
OBJ_ALL = $(OBJ_SIP_MSG) $(OBJ_MD5DIGEST) $(OBJ_PSA_SIP) $(OBJ_MSGLIB) 

msglib : $(OBJ_MSGLIB)
	$(CXX) -fPIC -shared -o $(DEPLOY_DIR)/app/sipmsglib.so $(OBJ_MSGLIB) $(LIBS)
	@echo "******** Finish making msglib ********"

sipmsgdef: $(OBJ_SIP_MSG)
	ar -crv $(LIB_DIR)/libsipmsgdef.a $(OBJ_SIP_MSG)
	@echo "******** Finish making sipmsgdef ********"

psasip: $(OBJ_MD5DIGEST) $(OBJ_PSA_SIP) 
	$(CXX) -fPIC -shared -o $(DEPLOY_DIR)/app/psasip.so $(OBJ_MD5DIGEST) $(OBJ_PSA_SIP) $(LIBS)
	@echo "******** Finish making psasip ***********"
#	
#md5digest: $(OBJ_MD5DIGEST)
#	ar -crv $(LIB_DIR)/libmd5digest.a $(OBJ_MD5DIGEST)
#	@echo "******** Finish making md5digest ********"

clean.sipmsgdef:
	-rm -f $(OBJ_SIP_MSG) $(LIB_DIR)/libsipmsgdef.a
	
clean.psasip:
	-rm -f $(OBJ_PSA_SIP) $(OBJ_MD5DIGEST) $(DEPLOY_DIR)/app/psasip.so

clean.msglib:
	-rm -f $(OBJ_MSGLIB) $(DEPLOY_DIR)/app/sipmsglib.so
#clean.md5digest:
#	-rm -f $(OBJ_MD5DIGEST) $(LIB_DIR)/libmd5digest.a


#=========== rtc_psa ===================
PSA_SRC_DIR = ./rtcpsa/source
OBJS_RTC_PSA = $(PSA_SRC_DIR)/CRoapParser.o \
		$(PSA_SRC_DIR)/CRtcProtocolParser.o \
		$(PSA_SRC_DIR)/CMsgBuffer.o \
		$(PSA_SRC_DIR)/CPracticalSocket.o \
		$(PSA_SRC_DIR)/CRtcStack.o \
		$(PSA_SRC_DIR)/CPsaRtc.o \
		$(PSA_SRC_DIR)/hookRtcImpl.o	
rtc_psa: $(OBJS_RTC_PSA)
	$(CXX) -shared -fPIC -o $(OUTPUT_DIR)/rtc_psa.so $(OBJS_RTC_PSA) $(LIBS)
	@echo "**************Finsh making rtc_PSA**********************"
clean.rtc_psa:
	rm -f $(OUTPUT_DIR)/rtc_psa.so $(OBJS_RTC_PSA)

#=========== test_server ===================
#OBJS_TEST_SERVER = ./server/source/testPSA.o
#test_server: $(OBJS_TEST_SERVER)
#	$(CXX) -shared -fPIC -o $(OUTPUT_DIR)/test_server.so $(OBJS_TEST_SERVER) $(LIBS)
#	@echo "**************Finsh making test_server**********************"
#clean.test_server:
#	rm -f $(OUTPUT_DIR)/test_server.so $(OBJS_TEST_SERVER)
	
#=========== rtc_msgdef ===================
## make compilemsg
MSGDEFS = msgdef_rtc.def 
#msgdatadef_rtc.def 
#compilemsg:
#	mmc $(MSGDEFS) -m msgdef_rtc/include -i /usr/local/include/mcf:msgdef_rtc/include -h msgdef_rtc/include -c msgdef_rtc/source -x msgdef_rtc/include

OBJS_RTC_MSGDEF = ./msgdef_rtc/source/msgdef_rtc.o
rtc_msgdef: $(OBJS_RTC_MSGDEF)
	ar -crv $(DEPLOY_DIR)/lib/librtcmsg.a $(OBJS_RTC_MSGDEF)
	@echo "*************** Finish making librtcmsg.a ***************"
clean.rtc_msgdef:
	rm -f $(OBJS_RTC_MSGDEF) $(OUTPUT_DIR)/librtcmsg.a
	
#=========== internal_msgdef ===================
## make compilemsg
MSGDEFS_INT = msgdef_internal.def 
#msgdatadef_rtc.def 
#compilemsg:
#	mmc $(MSGDEFS_INT) -m msgdef_internal/include -i /usr/local/include/mcf:msgdef_internal/include -h msgdef_internal/include -c msgdef_internal/source -x msgdef_internal/include

OBJS_INT_MSGDEF = ./msgdef_internal/source/msgdef_int.o
internal_msgdef: $(OBJS_INT_MSGDEF)
	ar -crv $(DEPLOY_DIR)/lib/libintmsg.a $(OBJS_INT_MSGDEF)
	@echo "*************** Finish making libintmsg.a ***************"
clean.internal_msgdef:
	rm -f $(OBJS_INT_MSGDEF) $(OUTPUT_DIR)/libintmsg.a

#=========== rtc_db ===================
OBJS_RTC_DB = ./dbhandler/source/dbHandler.o
rtc_db: $(OBJS_RTC_DB)
	ar -crv $(DEPLOY_DIR)/lib/librtcdb.a $(OBJS_RTC_DB)
	@echo "*************** Finish making librtcdb.a ***************"
clean.rtc_db:
	rm -f $(OBJS_RTC_DB) $(OUTPUT_DIR)/librtcdb.a

	
#=========== dispatcher ===================
OBJS_DISPATCHER = ./dispatcher/source/CDialogController.o ./dispatcher/source/CMsgDispatcher.o
dispatcher: $(OBJS_DISPATCHER)
	$(CXX) -shared -fPIC -o $(OUTPUT_DIR)/dispatcher.so $(OBJS_DISPATCHER) -L$(DEPLOY_DIR) $(LIBS) 
	@echo "**************Finsh making dispatcher.so**********************"
clean.dispatcher:	
	rm -f $(OBJS_DISPATCHER) $(OUTPUT_DIR)/dispatcher.so
	
	
#===============MESSAGE_XML_PARSE=================
OBJ_XML_PARSE = \
	./message_xml_parse/source/CMessageXmlParse.o\
	./message_xml_parse/tinyxml/source/tinyxml.o\
	./message_xml_parse/tinyxml/source/tinyxmlerror.o\
	./message_xml_parse/tinyxml/source/tinyxmlparser.o\
	./message_xml_parse/tinyxml/source/tinystr.o

	
clean.message_xml_parse:
	rm -f $(OBJ_XML_PARSE)
	
	
#=========== bearmodule ===================
compilebearsm:
	cd bear/nclude ; \
	java -jar ../../../tools/Smc.jar -c++ -d ../source -headerd . BearModule.sm

OBJS_BEAR = ./bear/source/CBearModule_sm.o \
	./bear/source/CBearModule.o
bear_module: $(OBJS_BEAR)
	$(CXX) -shared -fPIC -o $(OUTPUT_DIR)/bear_module.so $(OBJS_BEAR) -L$(DEPLOY_DIR) $(LIBS) -lsipmsgdef 
	@echo "**************Finsh making bear_module.so**********************"
clean.bear_module:
	rm -f $(OBJS_BEAR) $(OUTPUT_DIR)/bear_module.so
	
#=========== rtc_sip_callmodule ===================
compilesipsm:
	cd rtc_sip_callmodule/include ; \
	java -jar ../../tools/Smc.jar -c++ -d ../source -headerd . CRtcOrigCall.sm
	java -jar ../../tools/Smc.jar -c++ -d ../source -headerd . CSipTermCall.sm
#	cd ../../message_module/include ; \
#
OBJS_RTC_SIP_CALL = ./rtc_sip_callmodule/source/CR2SCallModule.o \
					./rtc_sip_callmodule/source/CRtcOrigCall_sm.o \
					./rtc_sip_callmodule/source/CSipTermCall_sm.o \
					./rtc_sip_callmodule/source/CUserMapHelper.o
					
rtc_sip_callmodule: $(OBJS_RTC_SIP_CALL)
	$(CXX) -shared -fPIC -o $(OUTPUT_DIR)/rtc_sip_callmodule.so $(OBJS_RTC_SIP_CALL) -L$(DEPLOY_DIR) $(LIBS) -lsipmsgdef -lintmsg
	@echo "**************Finsh making rtc_sip_callmodule.so**********************"
clean.rtc_sip_callmodule:
	rm -f $(OBJS_RTC_SIP_CALL) $(OUTPUT_DIR)/rtc_sip_callmodule.so
	
#=========== deploy =================
deploy: all
	mv ./rtc_psa.so $(DEPLOY_DIR)/app/rtc_psa.so
#	cp ./test_server.so $(DEPLOY_DIR)/app/test_server.so
	mv ./dispatcher.so $(DEPLOY_DIR)/app/dispatcher.so
	mv ./rtc_sip_callmodule.so $(DEPLOY_DIR)/app/rtc_sip_callmodule.so
	mv ./bear_module.so $(DEPLOY_DIR)/app/bear_module.so
	
	#cp $(D_SIPMSGDEF)/include/*.h /usr/local/include/mcf/sip
#	cp $(LIB_DIR)/libsipmsgdef.a /usr/local/lib/libmcfsipmsg.a
#	chmod 644 /usr/local/lib/libmcfsipmsg.a
	@echo "************* deploy finished*********************"

clean.deploy:
	rm -f $(DEPLOY_DIR)/app/rtc_psa.so
#	rm -f $(DEPLOY_DIR)/app/test_server.so
	rm -f $(DEPLOY_DIR)/app/dispatcher.so
	rm -f $(DEPLOY_DIR)/app/rtc_sip_callmodule.so
	rm -f $(DEPLOY_DIR)/lib/librtcmsg.a
	rm -f $(DEPLOY_DIR)/lib/librtcdb.a
	@echo "************* deploy clean finished*********************"
