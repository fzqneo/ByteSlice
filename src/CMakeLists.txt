list(APPEND byteslice-core_sources
    bitvector_block.cpp
    bitvector_iterator.cpp
    bitvector.cpp
    byteslice_column_block.cpp
    column.cpp
    naive_column_block.cpp
    sequential_binary_file.cpp
    types.cpp
    )

add_library(byteslice-core STATIC ${byteslice-core_sources})
