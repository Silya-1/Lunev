CC = gcc
CFLAGS =   -g -Wall -std=c99
GCOV_F = -fprofile-arcs -ftest-coverage
SOURCES = array.c $(EXECUTABLE).c
OBJECTS = $(SOURCES:.c = .o)
EXECUTABLE = test

all: $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC)   $(CFLAGS)  $(GCOV_F) -o $@ $^ -lm
%.o:%.c
	$(CC) -c -MD $(CFLAGS) $(GCOV_F) -o $@ $< -lm
-include $(wildcard *.d)
clean:
	rm -f *.o *.d *.gcda *.gcno main.out test
valgrind:
	valgrind ./$(EXECUTABLE)
gcov:
	gcovr -r ../
