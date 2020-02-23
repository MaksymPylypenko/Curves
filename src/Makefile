# For OS X
# How to use: change SRC_PREFIX to be the (unique) filenames of each of the
# source files that you want to build and link with the other source files
# in the directory. For example, if you have code in a1q1.cpp and a1q2.cpp
# then SRC_PREFIX=a1q and it will build and link with any source files not
# starting with that prefix (like main.cpp).
# The resulting executables will be placed in the OUT directory.

SRC_PREFIX=example

CC=clang++
CFLAGS=-Wall -std=c++11 -g -DDEBUG

SRC=./
OUT=../build
GLM=../glm
GLEW=../glew

LIBDIRS=-L$(GLEW)/lib
LIBS=-lGLEW
INCLUDES=-I$(GLM) -I$(GLEW)/include
FRAMEWORKS=-framework OpenGL -framework GLUT

programs = $(notdir $(basename $(wildcard $(SRC)/$(SRC_PREFIX)*)))
sources = $(filter-out $(wildcard $(SRC)/$(SRC_PREFIX)*),$(wildcard $(SRC)/*.cpp $(SRC)/*.c $(SRC)/*.C))
target_source := $(wildcard $(SRC)/$@.cpp $(SRC)/$@.c $(SRC)/$@.C)

all: $(programs)

$(SRC_PREFIX)%:	$(wildcard $(SRC)/$@.cpp $(SRC)/$@.c $(SRC)/$@.C) $(sources) $(wildcard $(SRC)/*.hpp $(SRC)/*.h $(SRC)/*.H)
	$(CC) $(CFLAGS) $(INCLUDES) $(LIBDIRS) $(LIBS) $(FRAMEWORKS) $(wildcard $(SRC)/$@.cpp $(SRC)/$@.c $(SRC)/$@.C) $(sources) -o $(OUT)/$@

clean:
	rm -f $(addprefix $(OUT)/,$(programs))
	rm -rf $(addsuffix .dSYM,$(addprefix $(OUT)/,$(programs)))
