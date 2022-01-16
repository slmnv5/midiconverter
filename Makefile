.PHONY: all info clean 

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TMP1 = $(shell find . -name "*.cpp" ! -name "test_*cpp")
OBJ_APP = $(TMP1:.cpp=.o)

TMP2 = $(shell find . -name "*.cpp" ! -name "mimap*cpp")
OBJ_TST =  $(TMP2:.cpp=.o)

TARGET = mimap
BUILD_MODE ?= debug

# added line 1
LDFLAGS += -pthread -lasound
# added line 2
CPPFLAGS += -std=c++11 
 

ifeq ($(BUILD_MODE),test)
	TARGET = test
	CFLAGS += -g
else ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else ifeq ($(BUILD_MODE),linuxtools)
	CFLAGS += -g -pg -fprofile-arcs -ftest-coverage
	LDFLAGS += -pg -fprofile-arcs -ftest-coverage
	EXTRA_CLEAN += mimap.gcda mimap.gcno $(PROJECT_ROOT)gmon.out
	EXTRA_CMDS = rm -rf mimap.gcda
else
    $(error Build mode $(BUILD_MODE) not supported by this Makefile)
endif

all: info $(TARGET) 


test: $(OBJ_TST)
	@echo "build test"
	$(CXX)  -o $@ $^  $(LDFLAGS)
	$(EXTRA_CMDS)

mimap: $(OBJ_APP)
	@echo "build app"
	$(CXX)  -o $@ $^  $(LDFLAGS)
	$(EXTRA_CMDS)

#%.o:	$(PROJECT_ROOT)%.cpp
#	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

 

clean:
	rm -frv $(TARGET) $(OBJ_APP) $(OBJ_TST) $(EXTRA_CLEAN)

	
info:
	cd $(PROJECT_ROOT)
	@echo LDFLAGS  $(LDFLAGS)
	@echo CPPFLAGS $(CPPFLAGS)
	@echo CXXFLAGS $(CXXFLAGS)
	@echo LDFLAGS $(LDFLAGS)
	@echo LDLIBS $(LDLIBS)
	@echo PROJECT_ROOT $(PROJECT_ROOT)
	@echo BUILD_MODE $(BUILD_MODE)
	@echo EXTRA_CLEAN $(EXTRA_CLEAN)
	@echo MAKEFILE_LIST $(MAKEFILE_LIST)
	@echo OBJ_APP $(OBJ_APP)
	@echo OBJ_TST $(OBJ_TST)
 
