add_executable(
    cli 
    main.cpp
    commands.cpp
)
target_link_libraries(cli PRIVATE engine)

