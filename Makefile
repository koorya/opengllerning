all: run

run: aplication.exe
	aplication.exe

aplication.exe: obj/main.o obj/shader.o
	g++ -o aplication.exe -Lc:/opengl/lib obj/*.o -lSOIL -lglew32s -llibglfw3 -lopengl32 -lgdi32 

obj/main.o: src/main.cpp
	g++ -c -Ic:/opengl/include src/main.cpp -o obj/main.o

obj/shader.o: src/shader.cpp src/shader.h
	g++ -c -Ic:/opengl/include src/shader.cpp -o obj/shader.o

clear:
	del obj\*.o