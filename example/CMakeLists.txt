list(APPEND examples
    example1
    )

foreach(ee ${examples})
    add_executable(${ee} "${ee}.cpp")
    target_link_libraries(${ee} byteslice-core)
endforeach()

