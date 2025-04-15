cmake_minimum_required(VERSION 3.16...3.26)  # Update to a more recent minimum version
project(OSM_A_star_search)

# Set C++ standard
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")

# Add cmake modules path for custom FindCairo.cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Find GraphicsMagick before adding io2d
find_package(GraphicsMagick REQUIRED)

# Disable building samples in io2d to avoid SVG sample build errors
set(IO2D_WITHOUT_SAMPLES ON CACHE BOOL "Don't build io2d samples" FORCE)

# Add thirdparty io2d library
add_subdirectory(thirdparty/P0267_RefImpl)

# Find required packages
find_package(Cairo REQUIRED)
find_package(io2d REQUIRED)

# Find GTest package
find_package(GTest REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CAIRO_INCLUDE_DIRS})
include_directories(${GraphicsMagick_INCLUDE_DIRS})
# Add the specific include path for io2d header files
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/P0267_RefImpl)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/P0267_RefImpl/P0267_RefImpl)

# Gather source files (excluding test files)
file(GLOB project_SRCS src/*.cpp model/*.cpp)
list(REMOVE_ITEM project_SRCS src/test.cpp)

# Create main executable
add_executable(OSM_A_star_search ${project_SRCS})
target_link_libraries(OSM_A_star_search
        ${CAIRO_LIBRARIES}
        ${GraphicsMagick_LIBRARIES}
        io2d::io2d
        pugixml
)

# Create test executable
file(GLOB test_SRCS test/*.cpp)
file(GLOB model_SRCS model/*.cpp)

add_executable(test
        ${test_SRCS}
        ${model_SRCS}
        src/model.cpp
        src/route_model.cpp
        src/route_planner.cpp
)

target_link_libraries(test
        ${CAIRO_LIBRARIES}
        ${GraphicsMagick_LIBRARIES}
        io2d::io2d
        pugixml
        GTest::GTest
        GTest::Main
)