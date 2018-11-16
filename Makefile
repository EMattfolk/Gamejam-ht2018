build:
	gcc -o bin/game src/core_basic_window.cpp -Iinc -Llib -lraylib -ldl -lglfw -lm -lX11 -lpthread

run: build
	./bin/game
