include(CheckCXXSourceRuns)
cmake_minimum_required(VERSION 3.10.2)
set(PROJECT_NAME minecraft-scanner)

project(${PROJECT_NAME} VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/obj)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Main program
file(GLOB_RECURSE SOURCE_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} CONFIGURE_DEPENDS
	${CMAKE_CURRENT_SOURCE_DIR}/src/**.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/**.hpp)
message("SOURCE_FILES ${SOURCE_FILES}")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

add_subdirectory(libs/libanvil anvil)
include_directories(libs/libanvil/include)
target_link_libraries(${PROJECT_NAME} libanvil)
target_link_libraries(${PROJECT_NAME} z)

include_directories(libs/indicators/single_include)
include_directories(libs/indicators/include)
include_directories(libs/fmt/include)

# Fake save
#add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#	COMMAND ${CMAKE_COMMAND} -E copy_directory
#	${CMAKE_SOURCE_DIR}/world $<TARGET_FILE_DIR:${PROJECT_NAME}>/world)