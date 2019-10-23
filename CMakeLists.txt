cmake_minimum_required(VERSION 3.5)
project(ArrowBase)
if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.1)
    set(CMAKE_CXX_STANDARD 14)
else ()
    set(CMAKE_CXX_STANDARD 11)
endif ()

include_directories(./utils)

if ((CMAKE_SYSTEM_NAME MATCHES "Linux") OR (CMAKE_SYSTEM_NAME MATCHES "Darwin"))
    set(PTHREAD -lpthread)
    if (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(AIO /usr/lib64/libaio.so -lrt)
    endif ()
    find_package(Boost 1.50.0 REQUIRED COMPONENTS program_options date_time filesystem iostreams)
    include_directories(${Boost_INCLUDE_DIR})
    if ((${Boost_VERSION} MATCHES "1.70.0") AND (CMAKE_SYSTEM_NAME MATCHES "Linux"))
        set(BOOST_PSEUDO "-lboost_program_options -lboost_date_time -lboost_iostreams")
    endif ()
else ()
    message("Windows cygwin")
    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    set(BOOST_ROOT D:/cygwin64)
    set(Boost_INCLUDE_DIR ${BOOST_ROOT}/usr/include)
    set(Boost_LIBRARY_DIR ${BOOST_ROOT}/lib)
    find_package(Boost 1.66.0 REQUIRED COMPONENTS program_options date_time filesystem iostreams)
    include_directories(${Boost_INCLUDE_DIR})
    link_directories(${Boost_LIBRARY_DIR})
endif ()

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
    enable_testing()
    find_package(gtest REQUIRED)
    include_directories(${GTEST_INCLUDE_DIRS})
    link_directories(/usr/local/lib)
else ()
    include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/../third-party/gtest-1.7.0/fused-src)
    include_directories(SYSTEM ${CMAKE_CURRENT_BINARY_DIR}/../third-party/gtest-1.7.0/fused-src/gtest)
    add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/../third-party/gtest-1.7.0/fused-src/gtest build)
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
        ${ARROW_PREFIX}/src/arrow/cpp/src/arrow/io
        ${ARROW_PREFIX}/src/arrow/cpp/src/parquet
        ${ARROW_PREFIX}/src/arrow-build/src
)

link_directories(${ARROW_PREFIX}/src/arrow-build/release)

set(ARROW_TEST
        unit/ArrowDummy.cpp
        efficiency/PseudoParquetWriterTest.cpp
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
    target_link_libraries(${CMAKE_PROJECT_NAME}_${exename}${ARTIFACT_SUFFIX} gtest -larrow -lparquet ${PTHREAD})
endforeach (sourcefile ${ARROW_TEST})