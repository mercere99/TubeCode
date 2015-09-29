CXX_nat := g++
CFLAGS_nat := -O3 -Wall
LFLAGS_nat := -ll -ly

CXX_web := emcc
OFLAGS_web := -g4 -DEMK_DEBUG
# OFLAGS_web := -oz
CFLAGS_web := -std=c++11 $(OFLAGS_web) --js-library library_tubecode.js -s EXPORTED_FUNCTIONS="['_emkMain', '_emkMain_tubecode', '_emkJSDoCallback', '_emkLoadString']"

default: native
all: native web

# What are the source files we are using?
SRC	:= inst.cc hardware.cc
OBJ	:= $(SRC:.cc=.o)

native: tubecode TubeIC

web:	tubecode.js TubeIC.js
web:	SRC += web_UI.cc

webIC:	TubeIC.js
webIC:	SRC += web_UI.cc


.cc.o:
	$(CXX_nat) $(CFLAGS_nat) -c $< -o $*.o

TubeIC.tab.cc: TubeIC.y
	bison -o TubeIC.tab.cc -b TubeIC -d TubeIC.y

TubeIC.yy.cc: TubeIC.lex
	flex -o TubeIC.yy.cc TubeIC.lex

tubecode.tab.cc: tubecode.y
	bison -o tubecode.tab.cc -b tubecode -d tubecode.y

tubecode.yy.cc: tubecode.lex
	flex -o tubecode.yy.cc tubecode.lex


TubeIC: TubeIC.tab.o TubeIC.yy.o $(OBJ)
	$(CXX_nat) $(CFLAGS_nat) -o TubeIC TubeIC.tab.o TubeIC.yy.o $(OBJ) $(LFLAGS_nat)

tubecode: tubecode.tab.o tubecode.yy.o $(OBJ)
	$(CXX_nat) $(CFLAGS_nat) -o tubecode tubecode.tab.o tubecode.yy.o $(OBJ) $(LFLAGS_nat)


TubeIC.js: TubeIC.tab.cc TubeIC.yy.cc $(SRC)
	$(CXX_web) $(CFLAGS_web) -o TubeIC.js TubeIC.tab.cc TubeIC.yy.cc $(SRC)

tubecode.js: tubecode.tab.cc tubecode.yy.cc $(SRC)
	$(CXX_web) $(CFLAGS_web) -o tubecode.js tubecode.tab.cc tubecode.yy.cc $(SRC)


clean:
	rm -f TubeIC.tab.cc TubeIC.tab.hh TubeIC.yy.cc TubeIC TubeIC.js tubecode.tab.cc tubecode.tab.hh tubecode.yy.cc tubecode tubecode.js *~ *.o *.js.map
