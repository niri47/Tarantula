include ../../../Makefile.inc

#through some shell magic, get the name of the current directory
PLUGINNAME = $(shell pwd |sed  's/\// /g' |awk '{print $$NF}')

FILES = $(shell ls -m *.cpp|sed 's/,//g')

EXTRAS = ../../../build/Tarantula-CallBackTools.o ../../../build/MouseCatcher-MouseCatcherSourcePlugin.o \
../../../build/MouseCatcher-MouseCatcherCore.o ../../../build/Tarantula-Device.o

COMMON = $(shell ls -m ../../../build/Common-*.o|sed 's/,//g')

all: $(addprefix ../../../bin/,$(addsuffix .so,$(PLUGINNAME)))

INCLUDE := ../../../include ../../../boost

../../../bin/$(PLUGINNAME).so : $(addprefix ../../../build/$(PLUGINNAME)-,$(FILES:.cpp=.o)) $(EXTRAS)
	$(CXX) $(COPTS) --shared -fPIC -o $@ $(COMMON) $(COMMON_OBJS)  $(addprefix ../../../build/$(PLUGINNAME)-,$(FILES:.cpp=.o)) $(EXTRAS) -L../../../bin -L../../../boost/libs -lboost_system -lpugixml

../../../build/$(PLUGINNAME)-%.o: %.cpp
	$(CXX) $(COPTS) --shared -fPIC -o $@ -c $<
