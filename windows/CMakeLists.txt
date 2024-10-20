cmake_minimum_required(VERSION 3.14)

set(PROJECT_NAME "jpeg_turbo_resizer")
project(${PROJECT_NAME} LANGUAGES CXX)

if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

cmake_policy(VERSION 3.14...3.25)

set(PLUGIN_NAME "jpeg_turbo_resizer_plugin")

list(APPEND PLUGIN_SOURCES
        "jpeg_turbo_resizer_plugin.cpp"
        "jpeg_turbo_resizer_plugin.h"
)

add_library(${PLUGIN_NAME} SHARED
        "include/jpeg_turbo_resizer/jpeg_turbo_resizer_plugin_c_api.h"
        "jpeg_turbo_resizer_plugin_c_api.cpp"
        ${PLUGIN_SOURCES}
)

apply_standard_settings(${PLUGIN_NAME})

set_target_properties(${PLUGIN_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden)
target_compile_definitions(${PLUGIN_NAME} PRIVATE FLUTTER_PLUGIN_IMPL)
################ EXIF
# Add exif-cpp source
list(APPEND PLUGIN_SOURCES
        "exif/exif.cpp"
)
# Add EXIF library
add_library(exif STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/exif/exif.cpp"
)

target_include_directories(exif PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/exif"
)

# Link EXIF library to your plugin
target_link_libraries(${PLUGIN_NAME} PRIVATE exif)
################## EXIF

target_include_directories(${PLUGIN_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(JPEG_TURBO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/libjpeg-turbo")
set(JPEG_TURBO_INCLUDE_DIR "${JPEG_TURBO_ROOT}/include")
set(JPEG_TURBO_LIBRARY_DIR "${JPEG_TURBO_ROOT}/lib")

message(STATUS "JPEG_TURBO_LIBRARY_DIR is set to: ${JPEG_TURBO_LIBRARY_DIR}")

include_directories(${JPEG_TURBO_INCLUDE_DIR})
link_directories(${JPEG_TURBO_LIBRARY_DIR})

target_link_libraries(${PLUGIN_NAME} PRIVATE flutter flutter_wrapper_plugin "${JPEG_TURBO_LIBRARY_DIR}/turbojpeg.lib")

add_custom_command(TARGET ${PLUGIN_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Checking for turbojpeg.dll..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${JPEG_TURBO_LIBRARY_DIR}/turbojpeg.dll"
        "$<TARGET_FILE_DIR:${PLUGIN_NAME}>/turbojpeg.dll"
)

set(jpeg_turbo_resizer_bundled_libraries
        "${JPEG_TURBO_LIBRARY_DIR}/turbojpeg.lib"
        PARENT_SCOPE
)

if (${include_${PROJECT_NAME}_tests})
    set(TEST_RUNNER "${PROJECT_NAME}_test")
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/release-1.11.0.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "Disable installation of googletest" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(${TEST_RUNNER}
            test/jpeg_turbo_resizer_plugin_test.cpp
            ${PLUGIN_SOURCES}
    )
    apply_standard_settings(${TEST_RUNNER})
    target_include_directories(${TEST_RUNNER} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    target_link_libraries(${TEST_RUNNER} PRIVATE flutter_wrapper_plugin)
    target_link_libraries(${TEST_RUNNER} PRIVATE gtest_main gmock)

    add_custom_command(TARGET ${TEST_RUNNER} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FLUTTER_LIBRARY}" $<TARGET_FILE_DIR:${TEST_RUNNER}>
    )

    include(GoogleTest)
    gtest_discover_tests(${TEST_RUNNER})
endif()