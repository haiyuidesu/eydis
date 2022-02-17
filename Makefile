.PHONY: all clean install debug

TARGET       = eydis

INST         = $(shell uname -m)
ARCH         = $(shell uname -s)
SRC          = $(wildcard src/*.c)

CFLAGS       += -Wall
CFLAGS       += -DSQLITE_ENABLE_FTS4 -Wall -Wno-format -Wunused-variable
CFLAGS       += -c -I. -O3 -g3 -o

LDFLAGS      =
LDLIBS       = -lsqlite3 -lreadline

INSTALL      = /usr/local/bin

OBJECTS      = $(SRC:.c=.o)

default : all

all : $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "LD 	$(TARGET)"
	@$(CC) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	@echo "OK: built $(TARGET) for $(ARCH) ($(INST))."

src/%.o: src/%.c
	@echo "CC 	$<"
	@$(CC) $< $(CFLAGS) $@

install: $(TARGET)
	@install -v $(TARGET) $(INSTALL)
	@echo "OK: installed $(TARGET)."

clean:
	@echo "OK: cleaned few files"
	@rm -rf $(TARGET) src/*.o .e*
