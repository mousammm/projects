CC = gcc

SRCDIR = src
BINDIR = build
BINARY = chip8

INCS = -I./src/includes
LIBS = -lSDL2

CFLAGS = -Wall -Wextra -g $(INCS)
LDLIBS = $(LIBS)

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BINDIR)/%.o, $(SRCS))

all: $(BINDIR)/$(BINARY)

$(BINDIR):
	mkdir -p $(BINDIR)

$(BINDIR)/$(BINARY): $(OBJS)
	@echo "Linking $@"
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

$(BINDIR)/%.o: $(SRCDIR)/%.c | $(BINDIR)
	@echo "Compiling $< -> $@"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm  -rf $(BINDIR)

.PHONY: all run clean
