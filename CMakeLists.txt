cmake_minimum_required (VERSION 3.9)
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
project (${PROJECT_NAME})
ADD_SUBDIRECTORY(imagequant)
ADD_SUBDIRECTORY(Source)


