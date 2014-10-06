
cc=gcc
CC=g++
cpp_flags=-g
link_flags=
include_path=
lib_path=
libs=-lpcrecpp -lpthread
objects=FormulaParser.o StrUtils.o Token.o TokenArray.o TokenStack.o
main_objs=main.o

excel : $(objects) $(main_objs)
	$(CC) -o excel $(link_flags) $(main_objs) $(objects) $(lib_path) $(libs)

$(objects) $(main_objs) : %.o: %.cpp 
	$(CC) -c $(include_path) $(cpp_flags) $< 

token: Token.o
array: TokenArray.o
stack: TokenStack.o

.PHONY : clean
clean :
	-rm excel $(objects) $(main_objs) $(test_objs) output *~ 2>/dev/null

