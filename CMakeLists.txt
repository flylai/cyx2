cmake_minimum_required(VERSION 3.11)
project(cyx2)

set(CMAKE_CXX_STANDARD 17)

option(CYX_DEBUG OFF)

add_subdirectory("src/3rdparty/googletest")
include_directories("src/3rdparty/dbg-macro" "src/3rdparty/googletest")
SET(GTEST_ROOT "src/3rdparty/googletest/googletest")

if (CYX_DEBUG)
    add_definitions(-D CYX_DEBUG)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif ()


add_definitions(-D DBG_MACRO_NO_WARNING)

file(GLOB_RECURSE CYX_COMPILER_SOURCE_FILES "src/compiler/*.cpp")
file(GLOB_RECURSE CYX_COMPILER_HEADER_FILES "src/compiler/*.hpp" "src/compiler/*.h")

file(GLOB_RECURSE CYX_VM_SOURCE_FILES "src/core/*.cpp")
file(GLOB_RECURSE CYX_VM_HEADER_FILES "src/core/*.hpp" "src/*.h")

file(GLOB_RECURSE CYX_OTHER_SOURCE_FILES "src/common/*.cpp" "src/utility/*.cpp")
file(GLOB_RECURSE CYX_OTHER_HEADER_FILES "src/common/*.h" "src/common/*.hpp" "src/utility/*.hpp")


add_executable(${PROJECT_NAME}
        ${CYX_COMPILER_HEADER_FILES}
        ${CYX_COMPILER_SOURCE_FILES}

        ${CYX_VM_HEADER_FILES}
        ${CYX_VM_SOURCE_FILES}

        ${CYX_OTHER_HEADER_FILES}
        ${CYX_OTHER_SOURCE_FILES}

        src/main.cpp
        )

add_executable(${PROJECT_NAME}_test
        test/run_test.cpp
        )
target_link_libraries(${PROJECT_NAME}_test
        gtest
        ${GTEST_LIBRARIES}
        )
