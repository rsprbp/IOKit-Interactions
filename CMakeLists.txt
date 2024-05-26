cmake_minimum_required(VERSION 3.10)
project(iokit-interactions C)

set(CMAKE_C_STANDARD 99)

add_executable(untitled iokit-interactions.c)

# Link the IOKit framework
target_link_libraries(untitled "-framework IOKit")

# Link the zlib library
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(untitled ${ZLIB_LIBRARIES})
endif()
