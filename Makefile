all: run

rebuild: 
	make clear
	make run

run: aplication.exe
	aplication.exe

aplication.exe: obj/main.o obj/shader.o obj/camera.o obj/mesh.o
	g++ -Wall -o aplication.exe -Lc:/opengl/lib obj/*.o -lassimp -lSOIL -lglew32s -llibglfw3 -lopengl32 -lgdi32 -static

obj/main.o: src/main.cpp 
	g++ -Wall -c -Ic:/opengl/include src/main.cpp -o obj/main.o

obj/shader.o: src/shader.cpp src/shader.h
	g++ -Wall -c -Ic:/opengl/include src/shader.cpp -o obj/shader.o


obj/camera.o: src/camera.cpp src/camera.h
	g++ -Wall -c -Ic:/opengl/include src/camera.cpp -o obj/camera.o

obj/mesh.o: src/mesh.cpp src/mesh.h
	g++ -Wall -c -Ic:/opengl/include src/mesh.cpp -o obj/mesh.o


clear:
	del obj\*.o
