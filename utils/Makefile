# Makefile for utils
# Author: WangChao
########################################################
#README:
#		will generate libutils.a, needed by other samples project
#       use make deploy to copy libutils.a to $UNIDIR
########################################################

OUTPUT_DIR = .
UNIDIR = ../unidir

INC_DIR = -I./include \
	-I/usr/local/include/mcf 

LIBS = -L. 

CXXFLAGS = -g -Wno-deprecated -Wno-write-strings -D_DEBUG $(INC_DIR)

%.o : %.C
	$(CXX) -c -o $@ $(CXXFLAGS) $<
#=========== USAGE ===================
usage:
	@echo "[Make Usage] :"
	@echo "    make all                             compile all"
	@echo "    make clean                           clean all"  
	@echo "    make deploy                          deploy libutils.a to UNIDIR/lib"
	@echo "    ********************************************************************"
	@echo "    UNIDIR:   $(UNIDIR)"
all : sample_util_lib 
clean: clean.sample_util_lib 

#=========== deploy =================
deploy: all
	cp $(OUTPUT_DIR)/libutils.a $(UNIDIR)/lib/libutils.a
	@echo "************* deploy finished*********************"

#=========== SAMPLE_UTIL_LIB =================
OBJ_UTIL_LIB = \
	./source/CProperties.o \
	./source/CPropertiesManager.o \
	./source/CTUniNetMsgHelper.o \
	./source/timerpoll.o

sample_util_lib : $(OBJ_UTIL_LIB)
	ar -crv $(OUTPUT_DIR)/libutils.a $(OBJ_UTIL_LIB)
	@echo "**************end of libutils**********************"
clean.sample_util_lib:
	rm -f $(OUTPUT_DIR)/libutils.a $(OBJ_UTIL_LIB)


