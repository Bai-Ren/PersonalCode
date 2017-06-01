MAX_EVENTS = 16

EXE = random.exe
LIN = random
OBJS = 

WINCC = x86_64-w64-mingw32-gcc
LINCC = gcc
WARNINGS = -Wall -Wextra -Wno-unused -Wno-unused-parameter
CFLAGS = -O2 $(WARNINGS) -std=c99

.PHONY: all
all: linux win

.PHONY: win
win: $(EXE)

.PHONY: linux
linux: $(LIN)

random.exe: random.c
	$(WINCC) $^ $(CFLAGS) -o $@ -D WIN

random: random.c
	$(LINCC) $^ $(CFLAGS) -o $@

.PHONY: clean
clean: 
	rm -f $(EXE) $(LIN) $(OBJS)