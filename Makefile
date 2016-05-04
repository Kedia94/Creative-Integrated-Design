CC=c++
CCFLAGS=-std=c++0x -g -O0 -pthread

SRC_DIR=src
OBJ_DIR=obj

SRC = RTSPServer.cpp \
      RTSPParser.cpp \
      RTPSender.cpp \
      Reader.cpp

OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: server

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

server: $(OBJ_DIR)/main.o $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $(OBJ_DIR)/main.o $(OBJ)


clean:
	rm -rf $(OBJ_DIR)/*.o server
