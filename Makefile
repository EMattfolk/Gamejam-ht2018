build:
	gcc -o bin/game src/Game.cpp -Iinc -Llib -lraylib -ldl -lglfw -lm -lX11 -lpthread

run: build
	./bin/game
