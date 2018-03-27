CC = g++
CXXFLAGS = --std=c++11 -Wall
SRC_DIR = src
INCLUDE_DIR = include
TARGET_DIR = target
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(TARGET_DIR)/%.o)
HEADERS = $(shell find $(INCLUDE_DIR) -name "*.hpp")

default_target: all

$(OBJS) : $(SRCS) $(HEADERS)
	$(CC) $(CXXFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

all : dir $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o test

dir :
	mkdir -p $(TARGET_DIR)

clean :
	rm -rf $(OBJS)

.ONESHELL :
deploy : clean
	git add .
	ifndef tag
	git commit -m "$(shell date +%Y/%m/%d-%R)"
	else
	git commit -m "$(tag)"
	fi
	git push
	
.PHONY : deploy clean all default_target