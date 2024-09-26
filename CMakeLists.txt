cmake_minimum_required(VERSION 3.5)

project(Portal)

add_compile_options(-O2)
add_subdirectory(cipher)
target_include_directories(cipher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/cipher)
add_executable(Portal main.cpp xenEncode.cpp memBlock.cpp)

target_link_libraries(Portal cipher)