CC = gcc
CFLAGS =   -g -Wall -O3  -std=c11 

SOURCES = integral.c $(EXECUTABLE).c
OBJECTS = $(SOURCES:.c = .o)
EXECUTABLE = integral

all: $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC)  $(CFLAGS)   $(GCOV_F) -o $@ $^ -lpthread -lm
%.o:%.c
	$(CC) -c  -MD $(CFLAGS) $(GCOV_F) -o $@ $< -lpthread -lm
-include $(wildcard *.d)
clean:
	rm -f *.o *.d integral
valgrind:
	valgrind ./$(EXECUTABLE)

