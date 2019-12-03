CFLAGS=-c -Wall #-g -O0
CXX_INCLUDES = -Ic:/opengl/include -IC:/opengl/lerning/nanoGUI/nanogui/ext/glad/include/
CXX_DEFINES = #-DGLAD_GLAPI_EXPORT -DNANOGUI_GLAD -DNANOGUI_SHARED -DNVG_SHARED 


all: run

rebuild: 
	make clear
	make run

run: aplication.exe
	aplication.exe


aplication.exe: obj/main.o obj/glad.o obj/shader.o obj/camera.o obj/mesh.o obj/model.o obj/load_tex.o obj/movement_program.o obj/manipulator.o obj/remote_manipulator.o obj/ConstructionContainer.o
	g++ -Wall -o aplication.exe -L.\ -Lc:/opengl/lib -L"C:\mysql-connector-c-6.0.2-mingw32\lib" obj/*.o -lassimp.dll -lSOIL -llibglfw3 -lopengl32 -lgdi32 -lmysqlclient -lws2_32 

obj/main.o: src/main.cpp 
	g++ $(CXX_DEFINES) $(CFLAGS) -DFULL_SCREEN_ $(CXX_INCLUDES) -I"C:\mysql-connector-c-6.0.2-mingw32\include" src/main.cpp  -o obj/main.o

obj/glad.o: C:\opengl\lerning\nanoGUI\nanogui\ext\glad\src\glad.c 
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) "C:\opengl\lerning\nanoGUI\nanogui\ext\glad\src\glad.c"  -o obj/glad.o


obj/shader.o: src/shader.cpp src/shader.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/shader.cpp -o obj/shader.o


obj/camera.o: src/camera.cpp src/camera.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/camera.cpp -o obj/camera.o

obj/mesh.o: src/mesh.cpp src/mesh.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/mesh.cpp -o obj/mesh.o

obj/model.o: src/model.cpp src/model.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/model.cpp -o obj/model.o

obj/load_tex.o: src/load_tex.cpp src/load_tex.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/load_tex.cpp -o obj/load_tex.o

obj/movement_program.o: src/movement_program.cpp src/movement_program.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/movement_program.cpp -o obj/movement_program.o

obj/manipulator.o: src/manipulator.cpp src/manipulator.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/manipulator.cpp -o obj/manipulator.o

obj/remote_manipulator.o: src/remote_manipulator.cpp src/remote_manipulator.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) -I"C:\mysql-connector-c-6.0.2-mingw32\include" src/remote_manipulator.cpp -o obj/remote_manipulator.o


obj/ConstructionContainer.o: src/ConstructionContainer.cpp src/ConstructionContainer.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/ConstructionContainer.cpp -o obj/ConstructionContainer.o



clear:
	del obj\*.o
