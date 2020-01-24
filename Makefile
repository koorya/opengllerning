CFLAGS=-c -Wall #-g -O0
CXX_INCLUDES = -Ic:/opengl/include -IC:/opengl/lerning/nanoGUI/nanogui/ext/glad/include/ -IC:/opengl/lerning/nanoGUI/nanogui/ext/eigen -IC:/opengl/lerning/nanoGUI/nanogui/ext/nanovg/src -IC:/opengl/lerning/nanoGUI/nanogui/include -IC:/opengl/nanogui 
CXX_DEFINES = -DNANOGUI_GLAD -DNANOGUI_SHARED -DNVG_SHARED #-DGLAD_GLAPI_EXPORT

#coment for git test
all: run

rebuild: 
	make clear
	make run

run: aplication.exe
	aplication.exe


aplication.exe: obj/main.o obj/glad.o obj/shader.o obj/camera.o obj/mesh.o obj/model.o obj/load_tex.o obj/movement_program.o obj/manipulator.o obj/main_frame.o obj/remote_manipulator.o obj/GUI_Manipulator.o obj/GUI_MainFrame.o obj/gui.o obj/ConstructionContainer.o
	g++ -Wall -o aplication.exe -L.\ -Lc:/opengl/lib -L"C:\mysql-connector-c-6.0.2-mingw32\lib" obj/*.o -lnanogui.dll -lassimp.dll -lSOIL -llibglfw3 -lopengl32 -lgdi32 -lmysqlclient -lws2_32 

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

obj/main_frame.o: src/main_frame.cpp src/main_frame.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/main_frame.cpp -o obj/main_frame.o	


obj/remote_manipulator.o: src/remote_manipulator.cpp src/remote_manipulator.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) -I"C:\mysql-connector-c-6.0.2-mingw32\include" src/remote_manipulator.cpp -o obj/remote_manipulator.o


obj/GUI_Manipulator.o: src/GUI_Manipulator.cpp src/GUI_Manipulator.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/GUI_Manipulator.cpp -o obj/GUI_Manipulator.o

obj/GUI_MainFrame.o: src/GUI_MainFrame.cpp src/GUI_MainFrame.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/GUI_MainFrame.cpp -o obj/GUI_MainFrame.o	

obj/gui.o: src/gui.cpp src/gui.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/gui.cpp -o obj/gui.o

obj/ConstructionContainer.o: src/ConstructionContainer.cpp src/ConstructionContainer.h
	g++ $(CXX_DEFINES) $(CFLAGS) $(CXX_INCLUDES) src/ConstructionContainer.cpp -o obj/ConstructionContainer.o



clear:
	del obj\*.o
