add_library(CountingQuotientFilter ../CountingQF.cpp)
add_library(Testing ../testFunctions.cpp)

target_include_directories(CountingQuotientFilter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(Testing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})