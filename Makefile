# This makefile was generated automatically, please do not edit
BUILD_DIR := build
DEST_DIR := .
GLOBAL_FLAGS := -Wall 
all: dirs dmake
dirs:
	mkdir -p $(BUILD_DIR)

clean:
	rm -f $(BUILD_DIR)/*
	rm -f $(DEST_DIR)/dmake

-include $(BUILD_DIR)/*.d

# target: dmake
dmake_FLAGS :=
dmake_OBJECTS :=$(BUILD_DIR)//main.cc_dmake.o $(BUILD_DIR)//parser.cc_dmake.o $(BUILD_DIR)//makefilegenerator.cc_dmake.o 

$(BUILD_DIR)//main.cc_dmake.o: src/main.cc
	g++ $(GLOBAL_FLAGS) -std=gnu++0x $(dmake_FLAGS) -c src/main.cc -o $(BUILD_DIR)//main.cc_dmake.o
$(BUILD_DIR)//parser.cc_dmake.o: src/parser.cc
	g++ $(GLOBAL_FLAGS) -std=gnu++0x $(dmake_FLAGS) -c src/parser.cc -o $(BUILD_DIR)//parser.cc_dmake.o
$(BUILD_DIR)//makefilegenerator.cc_dmake.o: src/makefilegenerator.cc
	g++ $(GLOBAL_FLAGS) -std=gnu++0x $(dmake_FLAGS) -c src/makefilegenerator.cc -o $(BUILD_DIR)//makefilegenerator.cc_dmake.o
# linux src files

dmake: $(dmake_OBJECTS) 
	$(CXX) -L$(DEST_DIR) -Wl,-rpath=$(DEST_DIR) $(dmake_OBJECTS) -o $(DEST_DIR)/dmake 

