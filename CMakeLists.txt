cmake_minimum_required(VERSION 3.23)

# set the project name
project(Mini_Shell C)

set(CMAKE_C_STANDARD 17)

# add the executable
add_executable(${PROJECT_NAME} main.c parser.c)

target_link_libraries(${PROJECT_NAME} readline)
