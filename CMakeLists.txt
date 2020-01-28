# Задать минимальную версию CMake:
cmake_minimum_required(VERSION 3.0)

# Указать характеристики проекта:
project(BuildingMachines VERSION 0.0.1 LANGUAGES CXX)

# Добавить в сборку исполняемый файл "MyProgram":
add_executable(MyProgram src/main.cpp)

#add_custom_target(run ALL cp ./MyProgram ../MyProgram && cd  ../ && optirun ./MyProgram )

add_subdirectory(ext/glad)
target_link_libraries(MyProgram glad)
target_include_directories(MyProgram PUBLIC glad/build/include)

target_include_directories(MyProgram PUBLIC /usr/include/mariadb)

target_link_libraries(MyProgram dl)
target_link_libraries(MyProgram pthread)

set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL " " FORCE)
add_subdirectory(ext/nanogui)
target_include_directories(MyProgram PUBLIC ext/nanogui/include)
target_include_directories(MyProgram PUBLIC ext/nanogui/ext/eigen)
target_include_directories(MyProgram PUBLIC ext/nanogui/ext/nanovg/src)

find_package(assimp REQUIRED)
find_package(glfw3 REQUIRED)
#надо проверить glm
# default-libmysqlclient-dev
#find_package(soil REQUIRED)
set(SOIL SOIL)

target_link_libraries(MyProgram assimp)
target_link_libraries(MyProgram glfw)
target_link_libraries(MyProgram GL)
target_link_libraries(MyProgram SOIL)
#undefined reference to `mysql_query'
target_link_libraries(MyProgram mariadb)

target_link_libraries(MyProgram nanogui)


# Скомпоновать исполняемый файл "MyProgram" со
# скомпилированной статической библиотекой "MyProgramCore":
#target_link_libraries(MyProgram MyProgramCore)

target_sources(MyProgram PUBLIC src/shader.cpp)
target_sources(MyProgram PUBLIC src/camera.cpp)
target_sources(MyProgram PUBLIC src/mesh.cpp)
target_sources(MyProgram PUBLIC src/model.cpp)
target_sources(MyProgram PUBLIC src/load_tex.cpp)
target_sources(MyProgram PUBLIC src/movement_program.cpp)
target_sources(MyProgram PUBLIC src/manipulator.cpp)
target_sources(MyProgram PUBLIC src/main_frame.cpp)
target_sources(MyProgram PUBLIC src/remote_manipulator.cpp)
target_sources(MyProgram PUBLIC src/GUI_Manipulator.cpp)
target_sources(MyProgram PUBLIC src/GUI_MainFrame.cpp)
target_sources(MyProgram PUBLIC src/gui.cpp)
target_sources(MyProgram PUBLIC src/ConstructionContainer.cpp)


# -L.\ 
# -Lc:/opengl/lib 
# -L"C:\mysql-connector-c-6.0.2-mingw32\lib" 

# nanogui.dll

# assimp.dll  sudo aptitude install libassimp-dev 
# SOIL        sudo aptitude install libsoil-dev
# glfw3    sudo aptitude install libglfw3-dev

# opengl32 - для винды надо этот оставить, только 64 битный. Он поставляется с компилятором libopengl32.a
# здесь вероятно это будет -lGL
# gdi32 - даже в винде не нужна, как оказалось

# mysqlclient - 
# ws2_32 - Для винового mysql




