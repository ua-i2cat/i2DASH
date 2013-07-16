CC = gcc
CFLAGS = -g -Wall -I$(INC_PATH)
LDFLAGS = -lm -lgpac -lavcodec -lswscale -lavformat -lavutil

SRC_PATH = src
OBJ_PATH = obj
INC_PATH = include
BIN_PATH = bin

OBJS = $(addprefix $(OBJ_PATH)/, context.o main.o sample.o i2dash.o)
PROG = i2test

vpath %c $(SRC_PATH)
vpath %o $(OBJ_PATH)
vpath %h $(INC_PATH)

all: $(PROG)

$(PROG): $(OBJS)
	@mkdir -p bin
	$(CC) $(CFLAGS) -o ./bin/$@ $(OBJS) $(LDFLAGS)

$(OBJ_PATH)/%.o: %.c
	@mkdir -p $(OBJ_PATH)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) ./bin/$(PROG)
