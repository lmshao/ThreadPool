CC = g++
LD = g++

SRC = $(wildcard *.cpp)
OBJ = $(patsubst %cpp, %o, $(SRC))

TARGET = ThreadPool-App

.PHONY: all clean
all: $(TARGET)

$(TARGET) : $(OBJ)
	$(LD) -o $@ $^ -lpthread -lboost_thread -lboost_system

%.o:%.cpp
	$(CC) -c $^

clean:
	rm -f $(OBJ) $(TARGET)
