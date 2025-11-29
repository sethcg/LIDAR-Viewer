INCLUDE(FetchContent)

# FETCH IMGUI (VCPKG INSTALL WAS NOT WORKING)
FetchContent_Declare(imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.3-docking
)
FetchContent_MakeAvailable(imgui)

# GET ALL RELEVANT SOURCE FILES
SET(SOURCES 
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

ADD_LIBRARY(imgui STATIC ${SOURCES})

TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC 
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

# PROVIDE NECESSARY SDL3 AND OPENGL LIBRARY
TARGET_LINK_LIBRARIES(imgui PUBLIC 
    SDL3::SDL3 
    OpenGL::GL
)

# FETCH TINY_FILE_DIALOGS
FetchContent_Declare(tinyfiledialog
    GIT_REPOSITORY https://github.com/native-toolkit/libtinyfiledialogs.git
    GIT_TAG 2.9.3
)
FetchContent_MakeAvailable(tinyfiledialog)

ADD_LIBRARY(tinyfiledialog STATIC ${tinyfiledialog_SOURCE_DIR}/tinyfiledialogs.c)
TARGET_INCLUDE_DIRECTORIES(tinyfiledialog PUBLIC ${tinyfiledialog_SOURCE_DIR})