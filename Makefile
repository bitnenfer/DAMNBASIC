CC = g++
FLR = -std=c++0x -Iinclude/
FLD = $(FLR) -D_DEBUG

all: debug

debug: _debug softclean
release: _release softclean

_release:
	mkdir -p temp/
	mkdir -p build/release/
	$(CC) -c src/DamnBasic.cpp $(FLR) -o temp/a.o
	$(CC) -c src/dbc/Builder.cpp $(FLR) -o temp/h.o
	$(CC) -c src/dbc/Debug.cpp $(FLR) -o temp/b.o
	$(CC) -c src/dbc/FileLoader.cpp $(FLR) -o temp/c.o
	$(CC) -c src/dbc/LexicalAnalyzer.cpp $(FLR) -o temp/d.o
	$(CC) -c src/dbc/SemanticAnalyzer.cpp $(FLR) -o temp/e.o
	$(CC) -c src/dbc/SyntaxAnalyzer.cpp $(FLR) -o temp/f.o
	$(CC) -c src/dbc/gen/ASM6502CG.cpp $(FLR) -o temp/g.o
	
	$(CC) temp/h.o temp/a.o temp/b.o temp/c.o temp/d.o temp/e.o temp/f.o temp/g.o -o build/release/dbc

_debug:
	mkdir -p temp/
	mkdir -p build/debug/
	$(CC) -c src/DamnBasic.cpp $(FLD) -o temp/a.o
	$(CC) -c src/dbc/Builder.cpp $(FLD) -o temp/h.o
	$(CC) -c src/dbc/Debug.cpp $(FLD) -o temp/b.o
	$(CC) -c src/dbc/FileLoader.cpp $(FLD) -o temp/c.o
	$(CC) -c src/dbc/LexicalAnalyzer.cpp $(FLD) -o temp/d.o
	$(CC) -c src/dbc/SemanticAnalyzer.cpp $(FLD) -o temp/e.o
	$(CC) -c src/dbc/SyntaxAnalyzer.cpp $(FLD) -o temp/f.o
	$(CC) -c src/dbc/gen/ASM6502CG.cpp $(FLD) -o temp/g.o
	
	$(CC) temp/h.o temp/a.o temp/b.o temp/c.o temp/d.o temp/e.o temp/f.o temp/g.o -o build/debug/dbc_debug
	
clean: hardclean

softclean:
	rm -f temp/*.o
	rm -rf temp/

hardclean: softclean
	rm -rf build/