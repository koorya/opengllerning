all: run

run: aplication.exe
	aplication.exe

aplication.exe: obj/main.o
	g++ -o aplication.exe -Lc:/opengl/lib obj/main.o -lglew32s -llibglfw3 -lopengl32 -lgdi32

obj/main.o: src/main.cpp
	g++ -c -Ic:/opengl/include src/main.cpp -o obj/main.o

clear:
	del obj\*.o