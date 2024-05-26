cmake_minimum_required(VERSION 3.10)
project(iokit C)

set(CMAKE_C_STANDARD 99)

# Define the executable and specify source files
add_executable(iokit iokit-interactions.c)

# Link the IOKit framework
target_link_libraries(iokit "-framework IOKit")

# Link the zlib library
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(iokit ${ZLIB_LIBRARIES})
endif()
