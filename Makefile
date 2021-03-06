CC = g++
CXXFLAGS = --std=c++11 -Wall -Wfloat-equal -Weffc++ -O3
LDFLAGS = -L /usr/local/lib -l citygml
SRC_DIR = src
INCLUDE_DIR = include
TARGET_DIR = target
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(TARGET_DIR)/%.o)
HEADERS = $(shell find $(INCLUDE_DIR) -name "*.h")
BIN = parser

# For Test
# Make Sure That GoogleTest Headers Exist
GTEST_INCLUDE_DIR = /usr/local/opt/googletest/include
# Make Sure That Googletest Obj Exist
GTEST_OBJ = /usr/local/opt/googletest/libgtest.a
TEST_DIR = test
TEST_TARGET_DIR = target-test
TEST_SRCS = $(shell find $(TEST_DIR) -name "*.cpp")
OBJS_EXCLUDE_MAIN = $(shell echo $(OBJS) | sed 's/target\/main.o//g')
TEST_BIN = unittest

ifndef tag
tag = $(shell date +%Y/%m/%d-%R)
endif

default_target: help
.PHONY : default_target

$(TARGET_DIR)/%.o : $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -I $(INCLUDE_DIR) -c $< -o $@

$(TEST_TARGET_DIR)/%.test.o : $(TEST_DIR)/%.cpp $(SRC_DIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CC) $(CXXFLAGS) -I $(INCLUDE_DIR) -I $(GTEST_INCLUDE_DIR) -c $< -o $@

all : dir $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)
.PHONY : all

dir :
	@mkdir -p $(TARGET_DIR)
	@mkdir -p $(TEST_TARGET_DIR)
.PHONY : dir

clean :
	rm -rf $(TARGET_DIR)
	rm -rf $(TEST_TARGET_DIR)
	rm -f $(BIN)
	rm -f $(TEST_BIN)
	rm -rf $(TEST_BIN).dSYM
.PHONY : clean

test-parser : dir $(TEST_TARGET_DIR)/parser.test.o $(OBJS_EXCLUDE_MAIN)
	$(CC) $(LDFLAGS) $(TEST_TARGET_DIR)/parser.test.o $(OBJS_EXCLUDE_MAIN) $(GTEST_OBJ) -o $(TEST_BIN)
.PHONY : test-parser

test-sampler : dir $(TEST_TARGET_DIR)/sampler/sampler.test.o $(OBJS_EXCLUDE_MAIN)
	$(CC) $(LDFLAGS) $(TEST_TARGET_DIR)/sampler/sampler.test.o $(OBJS_EXCLUDE_MAIN) $(GTEST_OBJ) -o $(TEST_BIN)
.PHONY : test-sampler

test-camera : dir $(TEST_TARGET_DIR)/camera.test.o $(OBJS_EXCLUDE_MAIN)
	$(CC) $(LDFLAGS) $(TEST_TARGET_DIR)/camera.test.o $(OBJS_EXCLUDE_MAIN) $(GTEST_OBJ) -o $(TEST_BIN)
.PHONY : test-camera

memcheck : test-camera
	valgrind --tool=memcheck --leak-check=full ./$(TEST_BIN)
.PHONY : memcheck

stats :
	find $(TEST_DIR) $(SRC_DIR) $(INCLUDE_DIR) -name "*.cpp" -o -name "*.h" | xargs wc -l
.PHONY : stats

deploy : clean
	git add .
	git commit -m "$(tag)"
	git push
.PHONY : deploy

help :
	@echo "Some Valid Targets For This Makefile:"
	@echo "... all"
	@echo "... clean"
	@echo "... test-parser"
	@echo "... test-sampler"
	@echo "... test-camera"
	@echo "... memcheck"
	@echo "... stats"
	@echo "... tag=\"\" deploy"
.PHONY : help

