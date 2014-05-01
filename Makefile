#
# Vincent Lantaca
# Makefile to make bit_detect.c
#

CC = gcc
#CFLAGS = -fnested-functions


BIT_DETECT_EXECUTABLE = bit_detect
BIT_DETECT_SOURCES = bit_detect.c
BIT_DETECT_OBJECTS = $(AUTOTUNE_SOURCES:.c=.o)



all: $(BIT_DETECT_SOURCES) $(BIT_DETECT_EXECUTABLE)

$(AUTOTUNE_EXECUTABLE): $(AUTOTUNE_OBJECTS) 
	$(CC) -lm $(LDFLAGS) $(AUTOTUNE_OBJECTS) -o $@

.cpp.o:
	$(CC) -lm $(CFLAGS) $< -o $@

clean:
	rm -rf $(SQUAREGEN_OBJECTS)
	rm -rf $(SQUAREGEN_EXECUTABLE)
