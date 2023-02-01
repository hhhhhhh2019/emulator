CC = g++ -I ./include -c -DDEBUG
LD = g++


SOURCES = emulator.cpp core.cpp utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)



%.o: %.cpp
	$(CC) $< -o $@

all: $(OBJECTS)
	$(LD) $^ -o emulator
	./emulator


clean:
	rm *.o emulator -rf
