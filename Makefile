CFLAGS=-c -Wall #-g -O0

all: run

rebuild: 
	make clear
	make run

run: aplication.exe
	aplication.exe



aplication.exe: obj/main.o obj/shader.o obj/camera.o obj/mesh.o obj/model.o obj/load_tex.o obj/movement_program.o obj/manipulator.o obj/remote_manipulator.o obj/ConstructionContainer.o
	g++ -Wall -o aplication.exe -Lc:/opengl/lib -L"C:\mysql-connector-c-6.0.2-mingw32\lib" obj/*.o -lassimp.dll -lSOIL -lglew32s -llibglfw3 -lopengl32 -lgdi32 -lmysqlclient -lws2_32

obj/main.o: src/main.cpp 
	g++ $(CFLAGS) -DFULL_SCREEN_ -Ic:/opengl/include -I"C:\mysql-connector-c-6.0.2-mingw32\include" src/main.cpp -o obj/main.o

obj/shader.o: src/shader.cpp src/shader.h
	g++ $(CFLAGS) -Ic:/opengl/include src/shader.cpp -o obj/shader.o


obj/camera.o: src/camera.cpp src/camera.h
	g++ $(CFLAGS) -Ic:/opengl/include src/camera.cpp -o obj/camera.o

obj/mesh.o: src/mesh.cpp src/mesh.h
	g++ $(CFLAGS) -Ic:/opengl/include src/mesh.cpp -o obj/mesh.o

obj/model.o: src/model.cpp src/model.h
	g++ $(CFLAGS) -Ic:/opengl/include src/model.cpp -o obj/model.o

obj/load_tex.o: src/load_tex.cpp src/load_tex.h
	g++ $(CFLAGS) -Ic:/opengl/include src/load_tex.cpp -o obj/load_tex.o

obj/movement_program.o: src/movement_program.cpp src/movement_program.h
	g++ $(CFLAGS) -Ic:/opengl/include src/movement_program.cpp -o obj/movement_program.o

obj/manipulator.o: src/manipulator.cpp src/manipulator.h
	g++ $(CFLAGS) -Ic:/opengl/include src/manipulator.cpp -o obj/manipulator.o

obj/remote_manipulator.o: src/remote_manipulator.cpp src/remote_manipulator.h
	g++ $(CFLAGS) -Ic:/opengl/include -I"C:\mysql-connector-c-6.0.2-mingw32\include" src/remote_manipulator.cpp -o obj/remote_manipulator.o


obj/ConstructionContainer.o: src/ConstructionContainer.cpp src/ConstructionContainer.h
	g++ $(CFLAGS) -Ic:/opengl/include src/ConstructionContainer.cpp -o obj/ConstructionContainer.o



clear:
	del obj\*.o
