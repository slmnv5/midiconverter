.PHONY:   info clean  build_debug build_run build_test run_test

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TMP1 = $(shell find . -name "*.cpp" ! -name "test_*cpp")
OBJ_APP = $(TMP1:.cpp=.o)

TMP2 = $(shell find . -name "*.cpp" ! -name "mimap*cpp")
OBJ_TST =  $(TMP2:.cpp=.o)

LDFLAGS += -pthread -lasound
CPPFLAGS += -std=c++11 -g
 
run_test: build_debug
	./mimap -c file_cont.txt -vv


build_test: $(OBJ_TST)
	@echo "build test and run all tests"
	cd $(PROJECT_ROOT)
	$(CXX)  -o $@ $^  $(LDFLAGS)


build_debug: info mimap
	@echo "build debug and run integration test"
	cd $(PROJECT_ROOT)

build_run: CPPFLAGS = -std=c++11 -O2
build_run: info mimap
	@echo "build release"
	cd $(PROJECT_ROOT)


mimap: $(OBJ_APP)
	@echo "build app"
	cd $(PROJECT_ROOT)
	$(CXX)  -o $@ $^  $(LDFLAGS)
 


DEPENDS = $(shell find . -name "*.d")

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@


-include $(DEPENDS)

clean:
	rm -frv  $(OBJ_APP) $(OBJ_TST)  

	
	
info:
	cd $(PROJECT_ROOT)
	@echo CXX  -- $(CXX)
	@echo CPPFLAGS -- $(CPPFLAGS)
	@echo CXXFLAGS -- $(CXXFLAGS)
	@echo LDFLAGS -- $(LDFLAGS)
	@echo LDLIBS -- $(LDLIBS)
	@echo PROJECT_ROOT -- $(PROJECT_ROOT)
	@echo MAKEFILE_LIST -- $(MAKEFILE_LIST)
	@echo OBJ_APP -- $(OBJ_APP)
	@echo OBJ_TST -- $(OBJ_TST)
	@echo DEPENDS -- ${DEPENDS}
 


 
