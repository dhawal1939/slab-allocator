add_executable(
    slow_io_slab_allocator main.cpp)


target_link_libraries(
    slow_io_slab_allocator PRIVATE slab_allocator )
