.PHONY: all clean install

GCC          = gcc
SRC          = src
TARGET       = eydis

INST         = $(shell uname -m)
ARCH         = $(shell uname -s)

CFLAGS       += 
CFLAGS       += -DDEBUG -DSQLITE_ENABLE_FTS4 -Wall -Wno-format
CFLAGS       += -c -I. -g3 -o

LDFLAGS      =
LDLIBS       = -lsqlite3 -lreadline

INSTALL      = /usr/local/bin

OBJECTS      = $(SRC)/utils.o \
               $(SRC)/shell.o \
               $(SRC)/instructions64.o \
               $(SRC)/database.o \
               $(SRC)/disarm.o \
               $(SRC)/eydis.o

default : all

all : $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "LD 	$(TARGET)"
	@$(GCC) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	@echo "OK: built $(TARGET) for $(ARCH) ($(INST))."

$(SRC)/%.o: $(SRC)/%.c
	@echo "GCC 	$<"
	@$(GCC) $< $(CFLAGS) $@

install: $(TARGET)
	@install -v $(TARGET) $(INSTALL)
	@echo "OK: installed $(TARGET)."

clean:
	@echo "OK: cleaned few files"
	@rm -rf $(TARGET) $(SRC)/*.o .e* $(INSN)/*.o
