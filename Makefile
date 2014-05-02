#
# Vincent Lantaca And Michael Bennett
# Makefile to make bit_detect.c and man_decode.c
#

CC = gcc
#CFLAGS = -fnested-functions


BIT_DETECT_EXECUTABLE = bit_detect
BIT_DETECT_SOURCES = bit_detect.c
BIT_DETECT_OBJECTS = $(AUTOTUNE_SOURCES:.c=.o)

MAN_DECODE_EXE = man_decode
MAN_DECODE_SOURCES = man_decode.c
MAN_DECODE_OBJECTS = $(AUTOTUNE_SOURCES:.c=.o)


all: $(BIT_DETECT_SOURCES) $(BIT_DETECT_EXECUTABLE) $(MAN_DECODE_SOURCES) $(MAN_DECODE_EXE)

$(AUTOTUNE_EXECUTABLE): $(AUTOTUNE_OBJECTS) 
	$(CC) -lm $(LDFLAGS) $(AUTOTUNE_OBJECTS) -o $@

.cpp.o:
	$(CC) -lm $(CFLAGS) $< -o $@

clean:
	rm -rf $(SQUAREGEN_OBJECTS)
	rm -rf $(SQUAREGEN_EXECUTABLE)
