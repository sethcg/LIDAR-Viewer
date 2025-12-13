IF(NOT TARGET lazperf)

    INCLUDE(FetchContent)

    SET(WITH_TESTS FALSE CACHE BOOL "")

    FetchContent_Declare(lazperf
        GIT_REPOSITORY https://github.com/hobuinc/laz-perf.git
        GIT_TAG 3.4.0
    )
    FetchContent_MakeAvailable(lazperf)

    TARGET_INCLUDE_DIRECTORIES(lazperf_s PUBLIC
        ${lazperf_SOURCE_DIR}/cpp
        ${lazperf_SOURCE_DIR}/cpp/lazperf
        ${lazperf_SOURCE_DIR}/cpp/lazperf/detail
    )

    ADD_LIBRARY(lazperf::shared ALIAS lazperf_s)

ENDIF()