SRC_PATH = src
OBJ_PATH = obj
INC_PATH = include
BIN_PATH = bin
CC:=gcc
INCLUDES:=$(shell pkg-config --cflags libavformat libavcodec libswscale libavutil sdl lgpac) 
CFLAGS:=-g -DDEBUG -Wall -I$(INC_PATH)
LDFLAGS:=$(shell pkg-config --libs libavformat libavcodec libswscale libavutil sdl) -lgpac -lm

OBJS = $(addprefix $(OBJ_PATH)/, context.o main.o segment.o fragment.o sample.o i2dash.o debug.o)
PROG = i2test

vpath %c $(SRC_PATH)
vpath %o $(OBJ_PATH)
vpath %h $(INC_PATH)

all: $(PROG)

$(PROG): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) $+ -o ./bin/$@ $(LDFLAGS)

$(OBJ_PATH)/%.o: %.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) ./bin/$(PROG)