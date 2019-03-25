CMAKE_MINIMUM_REQUIRED ( VERSION 2.6 )

project(hello_world_image)

set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

include_directories(include)

add_executable(
	main
	src/main.cpp
)
