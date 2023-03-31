hashtable_dir = ../hashtable/
holdall_dir = ../holdall/
ws_dir = ../ws/
CC = gcc
CFLAGS = -std=c18 \
  -Wall -Wconversion -Werror -Wextra -Wfatal-errors -Wpedantic -Wwrite-strings \
  -O2 -DHOLDALL_INSERT_TAIL \
  -I$(hashtable_dir) -I$(holdall_dir) -I$(ws_dir)
LDFLAGS =
vpath %.c $(hashtable_dir):$(holdall_dir):$(ws_dir)
vpath %.h $(hashtable_dir):$(holdall_dir):$(ws_dir)
objects = hashtable.o holdall.o ws.o main.o
executable = ws

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

hashtable.o: hashtable.c hashtable.h
holdall.o: holdall.c holdall.h
ws.o : ws.c ws.h
main.o: main.c hashtable.h holdall.h ws.h