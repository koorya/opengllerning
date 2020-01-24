# Задать минимальную версию CMake:
cmake_minimum_required(VERSION 3.0)

# Указать характеристики проекта:
project(BuildingMachines VERSION 0.0.1 LANGUAGES CXX)

# Добавить в сборку исполняемый файл "MyProgram":
add_executable(MyProgram StartProgram.c)

# Требовать обработку файла "core/CMakeFiles.txt":
add_subdirectory(ext/glad)
add_subdirectory(ext/nanogui)


# Скомпоновать исполняемый файл "MyProgram" со
# скомпилированной статической библиотекой "MyProgramCore":
#target_link_libraries(MyProgram MyProgramCore)



# main.cpp
# glad.cpp
# shader.cpp
# camera.cpp
# mesh.cpp
# model.cpp
# load_tex.cpp
# movement_program.cpp
# manipulator.cpp
# main_frame.cpp
# remote_manipulator.cpp
# GUI_Manipulator.cpp
# GUI_MainFrame.cpp
# gui.cpp
# ConstructionContainer.cpp


# -L.\ 
# -Lc:/opengl/lib 
# -L"C:\mysql-connector-c-6.0.2-mingw32\lib" 

nanogui.dll

assimp.dll  sudo aptitude install assimp-utils
SOIL        sudo aptitude install libsoil-dev
libglfw3    sudo aptitude install libglfw3-dev

# opengl32 - для винды надо этот оставить, только 64 битный. Он поставляется с компилятором libopengl32.a
# здесь вероятно это будет -lGL
# gdi32 - даже в винде не нужна, как оказалось

# mysqlclient - 
# ws2_32 - Для винового mysql




