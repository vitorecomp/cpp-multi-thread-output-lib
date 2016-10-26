CC = g++
HEADER_PATH = ./
SRC_PATH = ./
BIN_PATH = output/bin
REL_PATH = output/release
DEP_PATH = output/dep

_OBJ = output/output_test.o

_OBJ += json_parser/jsoncpp.o
_OBJ += output/IOMananger.o
_OBJ += output/ContextBuild.o
_OBJ += output/BaseTypes.o
_OBJ += output/Entities.o


OBJ = $(patsubst %,$(BIN_PATH)/%,$(_OBJ))

DIRECTIVES = -std=c++11 -w -c -I $(HEADER_PATH)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEP_PATH)/$*.Td
OPENCV = `pkg-config --libs opencv` -O4
LIBS = -pthread

RELEASE_NAME = output_test

FINAL_EXEC = $(REL_PATH)/$(RELEASE_NAME)
all: $(FINAL_EXEC)
$(FINAL_EXEC): $(OBJ)
	@echo Link All
	@mkdir -p $(REL_PATH)
	@$(CC) -o $@ $^ $(LIBS)
	@echo Everything OK

$(BIN_PATH)/%.o: $(SRC_PATH)/%.cpp
	@echo Compiling $<
	@mkdir -p $(DEP_PATH)/$(dir $*) $(BIN_PATH)/$(dir $*)
	@$(CC) $(DEPFLAGS) -c -o $@ $< $(DIRECTIVES)
	@mv -f $(DEP_PATH)/$*.Td $(DEP_PATH)/$*.d

debug: DIRECTIVES += -ggdb
debug: all

.PHONY: clean
clean:
	-@rm -r $(BIN_PATH)

$(DEP_PATH)/%.d: ;
.PRECIOUS: $(DEP_PATH)/%.d

-include $(patsubst %,$(DEP_PATH)/%.d,$(basename $(_OBJ)))
