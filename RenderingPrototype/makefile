default: main.c libs/shader.c libs/camera.c libs/inputOutput.c libs/dataImporter.c libs/graphics.c /opt/homebrew/opt/glad/src/glad.c
	clang -O3 -Wall -ffast-math -Wpedantic -Wpointer-arith -g \
	-I/opt/homebrew/opt/glfw/include -L/opt/homebrew/opt/glfw/lib \
	-I/opt/homebrew/opt/cglm/include -L/opt/homebrew/opt/cglm/include \
	-I/opt/homebrew/opt/glad/include -L/opt/homebrew/opt/glad/include -lglfw $^ -o test

quick: main.c libs/shader.c libs/camera.c libs/inputOutput.c libs/dataImporter.c libs/graphics.c /opt/homebrew/opt/glad/src/glad.c
	clang -O0 -Wall -ffast-math -Wpedantic -Wpointer-arith -g \
	-I/opt/homebrew/opt/glfw/include -L/opt/homebrew/opt/glfw/lib \
	-I/opt/homebrew/opt/cglm/include -L/opt/homebrew/opt/cglm/include \
	-I/opt/homebrew/opt/glad/include -L/opt/homebrew/opt/glad/include -lglfw $^ -o test

clean:
	rm -f test

debug: main.c libs/shader.c libs/camera.c libs/inputOutput.c libs/dataImporter.c libs/graphics.c /opt/homebrew/opt/glad/src/glad.c
	clang -Og -Wall -Wextra -Wundef -Wpedantic -Wpointer-arith -g -fsanitize=address,undefined -Wunreachable-code \
	-Wshadow -Wstrict-prototypes -Waggregate-return -Wno-unused-parameter \
	-I/opt/homebrew/opt/glfw/include -L/opt/homebrew/opt/glfw/lib \
	-I/opt/homebrew/opt/cglm/include -L/opt/homebrew/opt/cglm/include \
	-I/opt/homebrew/opt/glad/include -L/opt/homebrew/opt/glad/include -lglfw $^ -o test

run: test
	./test