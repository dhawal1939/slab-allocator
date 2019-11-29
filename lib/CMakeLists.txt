add_library(
	slab_allocator
	slab_allocator.hpp
	slab_allocator.cpp
	create_memory.hpp
	create_memory.cpp
	header.hpp)

target_include_directories(
	slab_allocator PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
)
