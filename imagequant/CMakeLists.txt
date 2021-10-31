cmake_minimum_required (VERSION 3.9)

project(imagequant C)

option(BUILD_WITH_SSE "Use SSE" ON)

if(BUILD_WITH_SSE)
  add_definitions(-DUSE_SSE=1)
endif()

find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()


if(CMAKE_COMPILER_IS_GNUCC)
  add_compile_options("-std=c99")
endif()

file (GLOB h_src "*.h" )
file (GLOB cpp_src "*.c" )


include_directories (
	.
)

# Make Source groups
source_group ("Header Files" FILES ${h_src})
source_group ("Source Files" FILES ${cpp_src})

add_library (${PROJECT_NAME} STATIC 
	${h_src} 
	${cpp_src} 
)

target_link_libraries (${PROJECT_NAME})

