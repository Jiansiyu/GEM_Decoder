CXX           =
ObjSuf        = o
SrcSuf        = cxx
ExeSuf        =
DllSuf        = so
OutPutOpt     = -o # keep whitespace after "-o"

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLDFLAGS  := $(shell root-config --ldflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)
HASTHREAD    := $(shell root-config --has-thread)

CXX           = g++
CXXFLAGS      = -O -Wall -fPIC
LD            = g++
LDFLAGS       = -O
SOFLAGS       = -shared

CXXFLAGS     += $(ROOTCFLAGS)
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(SYSLIBS)
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)

ROOTFLAGS    := ${LIBS} ${GLIBS} ${CXXFLAGS}
#------------------------------------------------------------------------------

TARGET   := ${patsubst %.cpp, %.o, ${wildcard *.cpp}} #${patsubst %.cpp, %.o, ${wildcard ./src/*.cpp}}
OBJECT   := ${wildcard *.cpp} #${wildcard ./src/*.cpp}
headers  := ${wildcard *.h}
USERLIBS := -I./headers -I./libs 

BUILD_AIM      := main

CC       := cc -std=c++11 ${USERLIBS} -Wall

${BUILD_AIM} : ${TARGET}
	echo "make ${TARGET}"
	echo "${OBJECT}"
	@$(CC) -pthread -g -o $@ $^ ${ROOTFLAGS} -lstdc++ -lm
	@${RM} ${TARGET} *.o

${TARGET} : ${OBJECT}
	@${CC} -c ${patsubst %.o, %.cpp, $@} ${ROOTFLAGS}

.PHONY: clean cleanall install help
clean :
	$(RM) ${TARGET} *~
cleanall :
	$(RM) ${BUILD_AIM} ${TARGET} *~ *.o
install :
	@echo "Waiting ......."
	@sleep 10
	@echo "Still waiting ????? "
	@sleep 3
	@echo "You stupid!"
help :
	@cat README



