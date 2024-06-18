# make -j$(expr $(nproc) / 2)
# ^ build with multi-threading to speed up the compilation

.PHONY: all clean install

CXX          = clang++
TARGET       = eydis

INST         = $(shell uname -m)
ARCH         = $(shell uname -s)
SRC          := $(shell find . -name "*.cpp")

CXXFLAGS     += -Wall -Wextra -Wpedantic
CXXFLAGS     += -DOPT5
CXXFLAGS     += -std=c++20 -I. -O2 -g3 -c

LDFLAGS      =
LDLIBS       =

INSTALL      = /usr/local/bin

OBJECTS      := $(SRC:%.cpp=%.o)

default: $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "LD 	$(TARGET)"
	@$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)
	@echo "[OK]: built $(TARGET) for $(ARCH) ($(INST))."

%.o: %.cpp
	@echo "CXX 	$<"
	@$(CXX) $< $(CXXFLAGS) -o $@

install: $(TARGET)
	@echo "[INFO]: installing $(TARGET) binary..."
	@install -v $(TARGET) $(INSTALL)
	@echo "[OK]: installed $(TARGET)."

clean:
	@rm -f $(TARGET) $(OBJECTS)
	@echo "[OK]: cleaned few files"
