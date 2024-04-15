cmake_minimum_required(VERSION 3.5)

## Creating the project name
set( PROJECT_NAME "Blockchain_Project_V3" )

## Set out project name
project(${PROJECT_NAME})

## Reading all the cpp files
FILE(GLOB SRCS  "${CMAKE_CURRENT_LIST_DIR}/src/blockchain/net/*.cpp" 
                "${CMAKE_CURRENT_LIST_DIR}/src/blockchain/storage/*.cpp" 
                "${CMAKE_CURRENT_LIST_DIR}/src/blockchain/*.cpp" 
                "${CMAKE_CURRENT_LIST_DIR}/src/*.cpp")

## Creating executables
add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ssl crypto)