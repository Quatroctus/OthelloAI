CC = g++ -g -std=c++11 
LIBS = -lglfw3 -ldl -lGL -l m -l X11 -pthread
INCLUDE = -L ./libraries/linux -I ./libraries/freetype/include/ -I ./libraries/glad/include/ -I ./libraries/glfw3/include/ -I ./libraries/glm/include/ -I ./libraries/stb_image/include/ ./libraries/linux/*.o

all:
	$(CC) $(INCLUDE) ./src/*.cpp -o othello $(LIBS)

clean:
	rm ./linux_obj/* ./othello
