.PHONY: info clean

PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
SRC_DIR := ./src

SRC_APP := $(shell find . -name "*.cpp" ! -name "test_*cpp")
SRC_TST := $(shell find . -name "*.cpp" ! -name "app_main*cpp")
OBJ_APP := $(SRC_APP:%=%.o)
OBJ_TST := $(SRC_TST:%=%.o)
DEPENDS := $(shell find . -name "*.d")

LDFLAGS := -pthread -lasound
CPPFLAGS := -I$(SRC_DIR) -MMD -MP
CXXFLAGS := -std=c++11 -g -Wno-psabi -Wall
 
app_t: $(OBJ_TST)
	@echo "build app for unit tests"
	cd $(PROJECT_ROOT)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^  $(LDFLAGS)


app_d: $(OBJ_APP)
	@echo "build app with debug settings"
	cd $(PROJECT_ROOT)
	@echo "build debug version"
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^  $(LDFLAGS)
	

app: CXXFLAGS = -std=c++11 -O2 -Wall
app: $(OBJ_APP)
	@echo "build app with release settings"
	cd $(PROJECT_ROOT)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $@ $^  $(LDFLAGS)
	mv -v app midiconverter
 
$(SRC_DIR)/pch.hpp.gch: $(SRC_DIR)/pch.hpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -x c++-header -c $< -o $@

$(SRC_DIR)/%.cpp.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/pch.hpp.gch
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@	
	
-include $(DEPENDS)

clean:
	cd $(PROJECT_ROOT)
	rm -fv  $(OBJ_APP) $(OBJ_TST) ${DEPENDS} mimap_t mimap_d mimap5 $(SRC_DIR)/pch.hpp.gch 

	
info:
	cd $(PROJECT_ROOT)
	@echo CXX  -- $(CXX)
	@echo CPPFLAGS -- $(CPPFLAGS)
	@echo CXXFLAGS -- $(CXXFLAGS)
	@echo LDFLAGS -- $(LDFLAGS)
	@echo LDLIBS -- $(LDLIBS)
	@echo PROJECT_ROOT -- $(PROJECT_ROOT)
	@echo SRC_APP -- $(SRC_APP)
	@echo SRC_TST -- $(SRC_TST)
	@echo OBJ_APP -- $(OBJ_APP)
	@echo OBJ_TST -- $(OBJ_TST)
	@echo DEPENDS -- ${DEPENDS}

 


 
