CC=c++
CCFLAGS=-std=c++0x -g -O0 -pthread

SRC_DIR=../src
OBJ_DIR=../obj
TEST_OBJ_DIR=obj
TEST_SRC_DIR=src

SRC = Server.cpp \
	  Client.cpp

OBJ = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: testServer testClient

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -c -o $@ $<

$(TEST_OBJ_DIR)/%.o: $(TEST_SRC_DIR)/%.cpp
	$(CC) $(CCFLAGS) -I$(SRC_DIR) -c -o $@ $<

testServer: $(TEST_OBJ_DIR)/testServer.o $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $(TEST_OBJ_DIR)/testServer.o $(OBJ)

testClient: $(TEST_OBJ_DIR)/testClient.o $(OBJ)
	$(CC) $(CCFLAGS) -o $@ $(TEST_OBJ_DIR)/testClient.o $(OBJ)

clean:
	rm -rf $(TEST_OBJ_DIR)/*.o testServer testClient
