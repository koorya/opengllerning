# Задать минимальную версию CMake:
cmake_minimum_required(VERSION 3.0)

# Указать характеристики проекта:
project(BuildingMachines VERSION 0.0.1 LANGUAGES CXX)

# Добавить в сборку исполняемый файл "${CMAKE_PROJECT_NAME}":
add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
#add_executable(${CMAKE_PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/example3.cpp)
add_definitions(-DNANOGUI_GLAD)


#add_custom_target(run ALL cp ./${CMAKE_PROJECT_NAME} ../${CMAKE_PROJECT_NAME} && cd  ../ && optirun ./${CMAKE_PROJECT_NAME} )


#add_subdirectory(ext/glad)
#target_link_libraries(${CMAKE_PROJECT_NAME} glad)
#target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_SOURCE_DIR}/build/ext/glad/include")

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC /usr/include/mariadb)

#target_link_libraries(${CMAKE_PROJECT_NAME} dl)
target_link_libraries(${CMAKE_PROJECT_NAME} pthread)

#
# пока воспользуемся glfw из nanogui, это неправильно, но 
# хочу запустить,чтоб хоть как-то собиралось
#
set(glfw3_DIR "C:\\Program Files (x86)\\GLFW\\lib\\cmake\\glfw3")
#find_package(glfw3 REQUIRED)
find_package(glfw3 3.3 REQUIRED)
message(STATUS "glfw include dir ${glfw3_INCLUDE_DIR}")
#target_link_libraries(${CMAKE_PROJECT_NAME} glfw)


#set(NANOGUI_BUILD_PYTHON OFF CACHE BOOL " " FORCE)
#add_subdirectory(ext/nanogui)
#add_definitions(-DNANOGUI_GLAD)
set(NANOGUI_BUILD_DIR "${CMAKE_SOURCE_DIR}/build/ext/nanogui")
set(NANOGUI_SRC_DIR "${CMAKE_SOURCE_DIR}/ext/nanogui")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${NANOGUI_SRC_DIR}/include")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${NANOGUI_SRC_DIR}/include/nanogui")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${NANOGUI_SRC_DIR}/ext/eigen")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${NANOGUI_SRC_DIR}/ext/nanovg/src")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${NANOGUI_SRC_DIR}/ext/glad/include")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${glfw3_DIR}/../../../include")
target_link_libraries(${CMAKE_PROJECT_NAME} "${NANOGUI_BUILD_DIR}/libnanogui.dll.a")

#target_link_libraries(${CMAKE_PROJECT_NAME} glfw)
#target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ext/nanogui/ext/glfw/include)

set(soil_DIR "C:/Program Files (x86)/SOIL")
target_link_libraries(${CMAKE_PROJECT_NAME} "${soil_DIR}/lib/libsoil.a")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${soil_DIR}/include")

set(glm_DIR "C:/src/glm")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${glm_DIR}")

add_definitions(-DKIRYA_WIN)
set(mysql_DIR "C:/src/mysql-connector-c-6.1.11-winx64")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${mysql_DIR}/include")
target_link_libraries(${CMAKE_PROJECT_NAME} "${mysql_DIR}/lib/libmysql.lib")



find_package(ASSIMP REQUIRED)
target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC ${ASSIMP_LIBRARY_DIRS})
target_link_libraries(${CMAKE_PROJECT_NAME} ${ASSIMP_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${ASSIMP_INCLUDE_DIRS})




# target_link_directories(${CMAKE_PROJECT_NAME} PUBLIC ${glfw3_LIBRARY_DIRS})
# target_link_libraries(${CMAKE_PROJECT_NAME} ${glfw3_LIBRARIES})
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${INTERFACE_INCLUDE_DIRECTORIES})
# message(STATUS "GLFW DIR: ${glfw3_DIR}")
# message(STATUS "GLFW INCLUDE DIR: ${glfw3_INCLUDE_DIRS}")
# message(STATUS "GLFW LIBRARIES: ${glfw3_LIBRARIES}")
# message(STATUS "GLFW LIBRARY DIRS: ${glfw3_LIBRARY_DIRS}")

#надо проверить glm
# default-libmysqlclient-dev
#find_package(soil REQUIRED)


target_link_libraries(${CMAKE_PROJECT_NAME} opengl32)
#target_link_libraries(${CMAKE_PROJECT_NAME} SOIL)
#undefined reference to `mysql_query'
#target_link_libraries(${CMAKE_PROJECT_NAME} mariadb)



target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/shader.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/camera.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/load_tex.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/movement_program.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/manipulator.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/main_frame.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/remote_manipulator.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/GUI_Manipulator.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/GUI_MainFrame.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/gui.cpp)
target_sources(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/ConstructionContainer.cpp)

# -L.\ 
# -Lc:/opengl/lib 
# -L"C:\mysql-connector-c-6.0.2-mingw32\lib" 

# nanogui.dll

# assimp.dll  sudo aptitude install libassimp-dev 
# SOIL        sudo aptitude install libsoil-dev
# glfw3    sudo aptitude install libglfw3-dev

# opengl32 - для винды надо этот оставить, только 64 битный. Он поставляется с компилятором libopengl32.a
# здесь вероятно это будет -lGL
# gdi32 - даже в винде не нужна, как оказалось. upd: нужна. эта штука нужна для загрузки функций из dll. в линуксе, я так понял используется для этого флаг -ld

# mysqlclient - 
# ws2_32 - Для винового mysql




