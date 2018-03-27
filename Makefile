CC := g++
CXXFLAGS := --std=c++11 -Wall
SRC_DIR := ./src
INCLUDE_DIR := ./include
SRCS := $(shell find $(SRC_DIR) -name "*.cpp")
OBJS := $(SRCS:.cpp=.o)
HEADERS := $(shell find $(INCLUDE_DIR) -name "*.hpp")

default_target: all

$(OBJS) : $(SRCS) $(HEADERS)
	@echo $(OBJS)
	@echo $(SRCS)
	@echo $(HEADERS)
	$(CC) $(CXXFLAGS) -I $(INCLUDE_DIR) $< -o $@

all : $(OBJS)
	$(CC) $(OBJS) -o $@

clean :
	rm -rf $(OBJS)
.PHONY : clean all default_target