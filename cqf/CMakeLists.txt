add_subdirectory(
	tests/Catch2/
)

add_executable(runTests tests/testSuite.cpp)

target_link_libraries(runTests 
	PRIVATE Catch2::Catch2WithMain
	PRIVATE cqf
)

add_library(cqf
	CountingQF.cpp
	filterUtils.cpp
)

target_include_directories(cqf
	PUBLIC
	"${PROJECT_SOURCE_DIR}/cqf/headers/"
)
