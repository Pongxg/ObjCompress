cmake_minimum_required (VERSION 3.9)

project (ObjCompress)
find_path(DRACO_DIR REQUIRED)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /DEBUG /OPT:REF /OPT:ICF")
set(CMAKE_CXX_STANDARD 17)

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
 # Find source files
#
set(draco_src_root "${CMAKE_CURRENT_SOURCE_DIR}/draco")


list(APPEND draco_io_sources
            "${draco_src_root}/io/file_reader_factory.cc"
            "${draco_src_root}/io/file_reader_factory.h"
            "${draco_src_root}/io/file_reader_interface.h"
            "${draco_src_root}/io/file_utils.cc"
            "${draco_src_root}/io/file_utils.h"
            "${draco_src_root}/io/file_writer_factory.cc"
            "${draco_src_root}/io/file_writer_factory.h"
            "${draco_src_root}/io/file_writer_interface.h"
            "${draco_src_root}/io/file_writer_utils.h"
            "${draco_src_root}/io/file_writer_utils.cc"
            "${draco_src_root}/io/mesh_io.cc"
            "${draco_src_root}/io/mesh_io.h"
            "${draco_src_root}/io/obj_decoder.cc"
            "${draco_src_root}/io/obj_decoder.h"
            "${draco_src_root}/io/obj_encoder.cc"
            "${draco_src_root}/io/obj_encoder.h"
            "${draco_src_root}/io/parser_utils.cc"
            "${draco_src_root}/io/parser_utils.h"
            "${draco_src_root}/io/ply_decoder.cc"
            "${draco_src_root}/io/ply_decoder.h"
            "${draco_src_root}/io/ply_encoder.cc"
            "${draco_src_root}/io/ply_encoder.h"
            "${draco_src_root}/io/ply_property_reader.h"
            "${draco_src_root}/io/ply_property_writer.h"
            "${draco_src_root}/io/ply_reader.cc"
            "${draco_src_root}/io/ply_reader.h"
            "${draco_src_root}/io/point_cloud_io.cc"
            "${draco_src_root}/io/point_cloud_io.h"
            "${draco_src_root}/io/stdio_file_reader.cc"
            "${draco_src_root}/io/stdio_file_reader.h"
            "${draco_src_root}/io/stdio_file_writer.cc"
            "${draco_src_root}/io/stdio_file_writer.h")

			  
			
file (GLOB h_src "*.h" )
file (GLOB cpp_src "*.cpp" )
file (GLOB nlohmann_h "nlohmann/*.h" )

include_directories (
	${DRACO_DIR}/include
	./
	.
)
link_directories(${DRACO_DIR}/lib)

set (ALL_LIBS imagequant draco)


# Make Source groups
source_group ("Header Files" FILES ${h_src})
source_group ("Source Files" FILES ${cpp_src})
source_group ("nlohmann" FILES ${nlohmann_h} )
source_group ("draco" FILES ${draco_io_sources})
# Main executable
#
add_executable (${PROJECT_NAME}  
	${h_src}
	${cpp_src} 
	${draco_io_sources}
)

target_link_libraries (${PROJECT_NAME} LINK_PUBLIC imagequant draco)

INSTALL (TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

