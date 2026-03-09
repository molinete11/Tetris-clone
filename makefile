
build:
	mkdir -p ./bin
	gcc src/*.c -o bin/tetris -L./vendor/ -lraylib -lm -ldl -lpthread -lGL -lX11 -O3 -march=native

clean:
	rm -r ./bin

run: build
	./bin/tetris

