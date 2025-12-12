IF(NOT TARGET lazperf)

    INCLUDE(FetchContent)

    # DISABLE TESTS
    SET(WITH_TESTS FALSE CACHE BOOL "")

    FetchContent_Declare(lazperf
        GIT_REPOSITORY https://github.com/hobuinc/laz-perf.git
        GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(lazperf)
ENDIF()