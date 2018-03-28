CC = g++
CXXFLAGS = --std=c++11 -Wall
LDFLAGS = -L /usr/local/lib -l citygml
SRC_DIR = src
INCLUDE_DIR = include
TARGET_DIR = target
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(TARGET_DIR)/%.o)
HEADERS = $(shell find $(INCLUDE_DIR) -name "*.h")
BIN = parser
ifndef tag
tag = $(shell date +%Y/%m/%d-%R)
endif

default_target: all

$(TARGET_DIR)/%.o : $(SRC_DIR)/%.cpp $(HEADERS)
	$(CC) $(CXXFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

all : dir $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)

dir :
	@mkdir -p $(TARGET_DIR)

clean :
	rm -rf $(OBJS)
	rm $(BIN)

deploy : clean
	git add .
	git commit -m "$(tag)"
	git push
	
.PHONY : deploy clean all default_target
