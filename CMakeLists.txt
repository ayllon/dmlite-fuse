cmake_minimum_required (VERSION 2.6)

project (dmlite-fuse)

# Definitions
add_definitions (-D_REENTRANT -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26)

# Build
add_executable (dmlite-fuse dmlite-fuse.c
                            io.c
                            ns.c)

target_link_libraries (dmlite-fuse dmlite fuse)
