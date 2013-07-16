CC = gcc
CFLAGS = -g -Wall -I$(INC_PATH)
LDFLAGS = -lgpac -lavcodec -lswscale -lavformat -lavutil

SRC_PATH = src
OBJ_PATH = obj
INC_PATH = include

OBJS = $(addprefix $(OBJ_PATH)/, context.o main.o sample.o i2dash.o)
PROG = i2test

vpath %c $(SRC_PATH)
vpath %o $(OBJ_PATH)
vpath %h $(INC_PATH)

all: $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) -o ./bin/$@ $(OBJS) $(LDFLAGS)

$(OBJ_PATH)/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) ./bin/$(PROG)
