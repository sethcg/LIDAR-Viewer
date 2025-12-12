IF(NOT TARGET tinyfiledialog)

    INCLUDE(FetchContent)

    FetchContent_Declare(tinyfiledialog
        GIT_REPOSITORY https://github.com/native-toolkit/libtinyfiledialogs.git
        GIT_TAG 2.9.3
    )
    FetchContent_MakeAvailable(tinyfiledialog)

    SET(SOURCES 
        ${tinyfiledialog_SOURCE_DIR}/tinyfiledialogs.c
    )

    ADD_LIBRARY(tinyfiledialog STATIC ${SOURCES})

    TARGET_INCLUDE_DIRECTORIES(tinyfiledialog PUBLIC 
        ${tinyfiledialog_SOURCE_DIR}
    )

ENDIF()