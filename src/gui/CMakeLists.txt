find_package(raylib REQUIRED)

set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/imgui")
set(RLIMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/rlImGui")

set(GUI_SOURCES
    src/main.cpp
    src/renderer.cpp

    # ImGui Core
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    # ImGui std::string support (highly recommended for your engine)
    ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
    # rlImGui Bridge
    ${RLIMGUI_DIR}/rlImGui.cpp
)
    
add_executable(gui ${GUI_SOURCES})

target_include_directories(gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/misc/cpp
    ${RLIMGUI_DIR}
)

target_link_libraries(gui PRIVATE engine raylib)

add_custom_command(TARGET gui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:gui>/assets"
)