#
#	Makefile
#

ASCMDAUDR32 = "~/projects/audr32/assembler/bin/assemble -o "

OUTAUDR32 = audr32-cwcc
SOURCE_DIR = src
BIN_DIR = bin
BUILD_DIR = build
CC = gcc
INCDIR = /tmp/include/
CFLAGS = -g -O0 -I$(SOURCE_DIR)/include -DINCDIR=\"$(INCDIR)\"
CFLAGS := $(CFLAGS) -DCWCC_AUDR32 -DASCMD=\"$(ASCMDAUDR32)\"

HEADERS = $(wildcard $(SOURCE_DIR)/include/*.h)
SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))

.PHONY: clean

all: installaudr32

$(BIN_DIR)/$(OUTAUDR32): $(OBJECTS) $(BUILD_DIR)/cg/codegen-audr32.o
	@printf "%8s %-40s %s\n" $(CC) $@ "$(CFLAGS)"
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CFLAGS) $^ -o $@
	@chmod +x $(BIN_DIR)/$(OUTAUDR32)

$(BUILD_DIR)/cg/%.o: $(SOURCE_DIR)/cg/%.c
	@printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)/cg/
	@$(CC) -c $(CFLAGS) -o $@ $<

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c $(HEADERS)
	@printf "%8s %-40s %s\n" $(CC) $< "$(CFLAGS)"
	@mkdir -p $(BUILD_DIR)/
	@$(CC) -c $(CFLAGS) -o $@ $<

installaudr32: $(BIN_DIR)/$(OUTAUDR32)
	mkdir -p $(INCDIR)
	rsync -a include/. $(INCDIR)
	cp $(BIN_DIR)/$(OUTAUDR32) ~/.local/bin
	chmod +x ~/.local/bin/$(OUTAUDR32)

clean:
	rm -r bin
	rm -r build
