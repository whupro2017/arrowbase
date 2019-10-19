cmake_minimum_required(VERSION 3.5)
project(ArrowBase)

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    enable_testing()
    find_package(gtest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
else ()
    #[[include_directories(SYSTEM ${PROJECT_SOURCE_DIR}/../../third-party/gtest-1.7.0/fused-src)
    include_directories(SYSTEM ${PROJECT_SOURCE_DIR}../../third-party/gtest-1.7.0/fused-src/gtest)
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/../../third-party/gtest-1.7.0/fused-src/gtest build)]]
endif ()

include(ExternalProject)
set(ARROW_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/arrow")
message(STATUS "BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "ARROW_PREFIX: ${ARROW_PREFIX}")
ExternalProject_Add(arrow
        PREFIX "${ARROW_PREFIX}"
        GIT_REPOSITORY https://github.com/apache/arrow
        GIT_TAG master
        UPDATE_COMMAND ""
        INSTALL_COMMAND ""
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DARROW_PARQUET=1
        CONFIGURE_COMMAND ${CMAKE_COMMAND} -DARROW_PARQUET=ON -DARROW_JEMALLOC=OFF -DARROW_BUILD_TESTS=OFF -DCMAKE_INSTALL_PREFIX:PATH=/usr/local ${ARROW_PREFIX}/src/arrow/cpp
        BUILD_COMMAND $(MAKE) -j8
        # Tell CMake to use subdirectory as source.
        LOG_DOWNLOAD ON
        LOG_CONFIGURE ON
        LOG_BUILD ON
        )

#[[file(GLOB_RECURSE arrowdirs ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/ CONFIGURE_DEPENDS "*.h")
message(${arrowdirs})]]

include_directories(
        arrow/unit
        ${arrowdirs}
        ${ARROW_PREFIX}/src/arrow/cpp/src
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/apdaters
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/array
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/compute
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/compute/kernel
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/compute/operations
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/csv
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/filesystem
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/dbi
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/testing
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/dataset
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/util
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/io
        ${ARROW_PREFIX}/src/arrow/cpp/src/parquet
        ${ARROW_PREFIX}/src/arrow-build/src
)

link_directories(${ARROW_PREFIX}/src/arrow-build/release)

set(ARROW_TEST
        unit/ArrowDummy.cpp
        )

foreach (sourcefile ${ARROW_TEST})
    get_filename_component(exename ${sourcefile} NAME_WE)
    add_executable(${CMAKE_PROJECT_NAME}_${exename}${ARTIFACT_SUFFIX} ${sourcefile})
    set_target_properties(${CMAKE_PROJECT_NAME}_${exename}${ARTIFACT_SUFFIX}
            PROPERTIES EXCLUDE_FROM_DEFAULT_BUILD_RELEASE 1
            EXCLUDE_FROM_DEFAULT_BUILD_MINRELEASE 1
            EXCLUDE_FROM_DEFAULT_BUILD_RELWITHDEBINFO 1
            OUTPUT_NAME ${exename}${ARTIFACT_SUFFIX}
            )
    add_dependencies(${CMAKE_PROJECT_NAME}_${exename}${ARTIFACT_SUFFIX} arrow)
    target_link_libraries(${CMAKE_PROJECT_NAME}_${exename}${ARTIFACT_SUFFIX} gtest -larrow ${PTHREAD})
endforeach (sourcefile ${ARROW_TEST})