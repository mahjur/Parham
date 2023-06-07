
SRC=parham.tab.o lex.yy.o main.o Cast.o Component.o Declaration.o Expression.o FieldList.o Identifier.o Mapper.o NameSpace.o NT.o Program.o Statement.o Type.o ConstructorManager.o ResolveManager.o

DEBUG=-g #-DDEBUG
INCLUDE=-I. -I../include

parham: $(SRC)
	g++ $(DEBUG) -o parham $(SRC)

%.o: %.cxx
	g++ $(DEBUG) $(INCLUDE) -c -Wall $<

%.o: ../src/%.cxx
	g++ $(DEBUG) $(INCLUDE) -c -Wall -Wno-cpp $<

%.o: %.c
	g++ $(DEBUG) $(INCLUDE) -c -Wall $<

parham.tab.cxx: ../src/parham.yxx
#	bison -d -r all $<
	bison -Wcounterexamples -d -r all $<

lex.yy.c: ../src/parham.lex
	flex $<

parham.tab.hxx: ../src/parham.yxx
	bison -d -r all $<

tags:
	ctags * ../include/*
	mv tags ../src/

clean:
	rm -f *.o *.hxx *.cxx *.c *.output parham

complete: clean parham

comp: clean parham

r: clean parham

