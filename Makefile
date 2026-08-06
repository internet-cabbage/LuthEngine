default: main.c tree.c sim.c galaxy.c io.c
	gcc -O3 -ffast-math -march=native -Wall -Werror -Wextra -g -Xpreprocessor -fopenmp \
	-Wconversion -Wunreachable-code -Wfloat-equal -Wshadow \
	-I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp $^ -o simulation -lm
	./simulation
experimental: main.c tree.c sim.c galaxy.c io.c
	clang -O3-ffast-math -march=native -Xpreprocessor -fopenmp \
	-I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp $^ -o simulation -lm
	./simulation
clean:
	rm -f default experimental experimentalgcc dataoutput