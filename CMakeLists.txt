cmake_minimum_required(VERSION 3.19)

project(CQF)

add_compile_options(-Wall -Werror -Ofast -g)

add_subdirectory(cqf)
add_subdirectory(apps)
