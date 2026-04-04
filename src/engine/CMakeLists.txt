add_library(engine STATIC 
    board.cpp
    precomputed.cpp
    move_generator.cpp
    magics.cpp
    diagnostics.cpp
    evaluate.cpp
    search.cpp
)

target_include_directories(engine 
    PUBLIC 
        ${CMAKE_SOURCE_DIR}/include
)

# target_compile_options(engine PRIVATE
#     -Wall
#     -Wextra
#     -Wpedantic
#     -Wconversion
#     -Wsign-conversion
# )