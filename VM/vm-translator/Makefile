all: bin/VMtranslator

bin/VMtranslator: build/VM_translator.o build/codewriter.o build/parser.o
	gcc build/VM_translator.o build/codewriter.o build/parser.o -o bin/VMtranslator

build/VM_translator.o: src/VM_translator.c src/codewriter/codewriter.h src/parser/parser.h
	gcc -Wall -g -c src/VM_translator.c -o build/VM_translator.o

build/codewriter.o: src/codewriter/codewriter.c src/codewriter/codewriter.h
	gcc -Wall -g -c src/codewriter/codewriter.c -o build/codewriter.o

build/parser.o: src/parser/parser.c src/parser/parser.h
	gcc -Wall -g -c src/parser/parser.c -o build/parser.o

clean:
	rm -f ./bin/* ./build/*