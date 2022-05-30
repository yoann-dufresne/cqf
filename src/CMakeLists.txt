cmake_minimum_required(VERSION 3.22.1)

project(CQF)

add_executable(${PROJECT_NAME} main.cpp)
add_compile_options(-Wall -Werror -Ofast)
add_subdirectory(headers)

target_include_directories(${PROJECT_NAME} PUBLIC headers)
target_link_directories(${PROJECT_NAME} PRIVATE headers)
target_link_libraries(${PROJECT_NAME} CountingQuotientFilter)
target_link_libraries(${PROJECT_NAME} Testing)