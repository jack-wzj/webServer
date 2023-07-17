file(GLOB_RECURSE pine_sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_library(my_shared SHARED ${pine_sources})