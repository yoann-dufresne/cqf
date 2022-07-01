
add_library(cqf
	CountingQF.cpp
	filterUtils.cpp
)

target_include_directories(cqf
	PUBLIC
	"${PROJECT_SOURCE_DIR}/cqf/headers/"
)
