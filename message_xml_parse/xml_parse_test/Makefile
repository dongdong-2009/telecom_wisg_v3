#Makefile for tinyxml
#Author: Zhangpeng
#last-modified-time 2011-09-22
###############################################
#README:
#	this program is for the test of message_xml_parse
#
###############################################
OUTPUT_DIR = .
CURRENT_DIR = .
COMPILE_DIR = ..

INC_DIR = -I$(COMPILE_DIR)/include \
	-I$(COMPILE_DIR)/tinyxml/include \

CXXFLAGS = $(INC_DIR)

%.o : %.C
	$(CXX) -c -o $@ $(CXXFLAGS) $<


#===========all============================
OBJ_XML_PARSE = $(COMPILE_DIR)/tinyxml/source/tinystr.o \
		$(COMPILE_DIR)/tinyxml/source/tinyxml.o \
		$(COMPILE_DIR)/tinyxml/source/tinyxmlerror.o \
		$(COMPILE_DIR)/tinyxml/source/tinyxmlparser.o\
		$(COMPILE_DIR)/source/CMessageXmlParse.o\
		$(CURRENT_DIR)/test.o

test:$(OBJ_XML_PARSE)
	$(CXX) $(CXXFLAGS) -o $(OUTPUT_DIR)/test $(OBJ_XML_PARSE)

#=============clean===========================
clean:
	rm -f $(OBJ_XML_PARSE) $(OUTPUT_DIR)/test

