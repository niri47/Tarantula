include ../../../Makefile.inc

#through some shell magic, get the name of the current directory
PLUGINNAME = $(shell pwd |sed  's/\// /g' |awk '{print $$NF}')

FILES = $(shell ls -m *.cpp)

EXTRAS = ../../../build/Common-TarantulaPlugin.o ../../../build/Common-Log.o

all: $(addprefix ../../../bin/,$(addsuffix .so,$(PLUGINNAME)))

INCLUDE := ../../../include ../../../boost

../../../bin/$(PLUGINNAME).so : $(addprefix ../../../build/$(PLUGINNAME)-,$(FILES:.cpp=.o)) $(EXTRAS)
	$(CXX) $(COPTS) --shared -fpic -o $@ $(COMMON_OBJS)  $(addprefix ../../../build/$(PLUGINNAME)-,$(FILES:.cpp=.o)) $(EXTRAS)

../../../build/$(PLUGINNAME)-%.o: %.cpp
	$(CXX) $(COPTS) --shared -fpic -o $@ -c $<