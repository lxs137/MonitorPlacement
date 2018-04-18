CMAKE_MINIMUM_REQUIRED(VERSION 3.6)

SET(GTEST_INCLUDE "/usr/local/opt/googletest/include")
SET(GTEST_LIBRARY "/usr/local/opt/googletest/libgtest.a")
SET(UNITTEST_DIR "${CMAKE_CURRENT_LIST_DIR}/test")
option(test "Build all tests." ON)

PROJECT(MonitorPlacement)

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED on)
ADD_COMPILE_OPTIONS(-Wall)
ADD_COMPILE_OPTIONS(-ggdb)

SET(CMAKE_EXE_LINKER_FLAGS "-L /usr/local/lib -l citygml")

SET(CMAKE_CXX_FLAGS_DEBUG "-O0")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_LIST_DIR}/include)
INCLUDE_DIRECTORIES(/usr/local/include)
INCLUDE_DIRECTORIES(/usr/local/opt/googletest/include)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR}/src/voxelizer DIR_SRC)
AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR}/src DIR_SRC)
LIST(REMOVE_ITEM DIR_SRC ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp)

ADD_LIBRARY(parser_lib ${DIR_SRC})
ADD_EXECUTABLE(parser ./src/main.cpp)

TARGET_LINK_LIBRARIES(parser parser_lib)

if (APPLE)
    ADD_DEFINITIONS(-DGTEST_USE_OWN_TR1_TUPLE)
    ADD_DEFINITIONS(-D__GLIBCXX__)
endif (APPLE)

ENABLE_TESTING()

INCLUDE_DIRECTORIES(${GTEST_INCLUDE})

##############
# Unit Tests
##############
ADD_EXECUTABLE(unittest ${UNITTEST_DIR}/parser.cpp)

TARGET_LINK_LIBRARIES(unittest ${GTEST_LIBRARY} parser_lib)

add_test(NAME that-test-I-made COMMAND unittest)

add_test(that-other-test-I-made unittest)