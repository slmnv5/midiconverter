.PHONY: info clean extraclean

PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

TMP1 = $(shell find . -name "*.cpp" ! -name "test_*cpp")
OBJ_APP = $(TMP1:.cpp=.o)

TMP2 = $(shell find . -name "*.cpp" ! -name "app_main*cpp")
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
	./start_test.sh
	
	

mimap5: CPPFLAGS = -std=c++11 -O2
mimap5: $(OBJ_APP)
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
	cd $(PROJECT_ROOT)
	rm -fv  $(OBJ_APP) $(filter-out ./test_main.o, $(OBJ_TST)) ${DEPENDS} mimap_t mimap_d mimap5

extraclean:
	cd $(PROJECT_ROOT)
	rm -fv  ./*.o ./*.d pch.hpp.gch mimap_t mimap_d mimap5
	
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
 


 
