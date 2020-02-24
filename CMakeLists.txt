cmake_minimum_required(VERSION 3.10)

project(boolean-solutor
    LANGUAGES CXX C
    VERSION 1.0.0.0
)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(SOURCE_FILES
    src/main.cpp
    src/BooleanSolutor.cpp
)

set(RESOURCE_FILES
    src/log4c.properties
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC log4cplus)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

configure_file (${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/)
