.PHONY: info clean build_debug build_run run_test

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TMP1 = $(shell find . -name "*.cpp" ! -name "test_*cpp")
OBJ_APP = $(TMP1:.cpp=.o)

TMP2 = $(shell find . -name "*.cpp" ! -name "mimap*cpp")
OBJ_TST =  $(TMP2:.cpp=.o)

LDFLAGS += -pthread -lasound
CPPFLAGS += -std=c++11 -g
 

mimap_t: $(OBJ_TST)
	@echo "build app test and run unit tests"
	cd $(PROJECT_ROOT)
	$(CXX)  -o $@ $^  $(LDFLAGS)
	./mimap_t

mimap_d: $(OBJ_APP)
	cd $(PROJECT_ROOT)
	@echo "build app debug and run integration test"
	$(CXX)  -o $@ $^  $(LDFLAGS)
	./start.sh
	
	

mimap: CPPFLAGS = -std=c++11 -O2
mimap: $(OBJ_APP)
	cd $(PROJECT_ROOT)
	@echo "build app"
	$(CXX)  -o $@ $^  $(LDFLAGS)
 
pch.hpp.gch: pch.hpp
	$(CXX) $(CXXFLAGS) -x c++-header -c $< -o $@

 
DEPENDS = $(shell find . -name "*.d")

%.o: %.cpp pch.hpp.gch
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

-include $(DEPENDS)

clean:
	rm -fv  $(OBJ_APP) $(filter-out ./test_0.o, $(OBJ_TST)) ${DEPENDS} mimap_t mimap_d mimap

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
 


 
