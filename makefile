
build:
	mkdir ./bin
	gcc src/*.c -o bin/tetris -L./vendor/ -lraylib -lm -ldl -lpthread -lGL -lX11

clean:
	rm -r ./bin

run:
	./bin/tetris

