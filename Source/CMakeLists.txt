cmake_minimum_required (VERSION 3.9)

project (ObjCompress)
find_path(DRACO_DIR REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_CXX_STANDARD 14)

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
 # Find source files
#

file (GLOB h_src "*.h" )
file (GLOB cpp_src "*.cpp" )

include_directories (
	${DRACO_DIR}/include
	.
)
link_directories(${DRACO_DIR}/lib)

set (ALL_LIBS imagequant draco)


# Make Source groups
source_group ("Header Files" FILES ${h_src})
source_group ("Source Files" FILES ${cpp_src})

# Main executable
#
add_executable (${PROJECT_NAME}  
	${h_src}
	${cpp_src} 
)

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC imagequant draco)

INSTALL (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

