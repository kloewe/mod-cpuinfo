project(cpuinfo)

add_compile_options(-std=c99 -Wall -Wextra -Wno-unused-parameter -Wconversion -Wshadow -pedantic)

add_library(cpuinfo src/cpuinfo.c)
target_compile_definitions(cpuinfo PRIVATE -DNDEBUG)
