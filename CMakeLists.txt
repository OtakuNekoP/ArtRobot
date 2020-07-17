cmake_minimum_required(VERSION 3.4)

set (CMAKE_CXX_STANDARD 11)

# set(CMAKE_EXE_LINKER_FLAGS "-static -v")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTIMER=1")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")

project(ArtRobot)

# libs
include(FindPkgConfig)

## cairo
pkg_check_modules(CAIRO cairo)
if (NOT CAIRO_FOUND)
message(FATAL_ERROR "Cairo not found!")
endif ()
message(STATUS "Cairo library status:")
message(STATUS "    version: ${CAIRO_VERSION}")
message(STATUS "    include path: ${CAIRO_INCLUDE_DIRS}")
message(STATUS "    lib path: ${CAIRO_LIBDIR}")
message(STATUS "    libraries: ${CAIRO_LIBRARIES}")

## pango
pkg_check_modules(PANGO pangocairo)
if (NOT CAIRO_FOUND)
message(FATAL_ERROR "Pango not found!")
endif ()
message(STATUS "Pango library status:")
message(STATUS "    version: ${PANGO_VERSION}")
message(STATUS "    include path: ${PANGO_INCLUDE_DIRS}")
message(STATUS "    lib path: ${PANGO_LIBDIR}")
message(STATUS "    libs path: ${PANGO_LIBRARY_DIRS}")
message(STATUS "    libraries: ${PANGO_LIBRARIES}")

## rsvg
pkg_check_modules(RSVG librsvg-2.0)
if (NOT RSVG_FOUND)
message(FATAL_ERROR "rsvg not found!")
endif ()
message(STATUS "rsvg library status:")
message(STATUS "    version: ${RSVG_VERSION}")
message(STATUS "    include path: ${RSVG_INCLUDE_DIRS}")
message(STATUS "    lib path: ${RSVG_LIBDIR}")
message(STATUS "    libs path: ${RSVG_LIBRARY_DIRS}")
message(STATUS "    libraries: ${RSVG_LIBRARIES}")

# Find OpenCV
find_package(OpenCV REQUIRED)
message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "    libraries: ${OpenCV_LIBS}")

## libwebp
pkg_check_modules(WEBP libwebp)
if (NOT WEBP_FOUND)
message(FATAL_ERROR "libwebp not found!")
endif ()
message(STATUS "libwebp library status:")
message(STATUS "    version: ${WEBP_VERSION}")
message(STATUS "    include path: ${WEBP_INCLUDE_DIRS}")
message(STATUS "    lib path: ${WEBP_LIBDIR}")
message(STATUS "    libraries: ${WEBP_LIBRARIES}")

# Project
set(SRC_LIST
    src/Component/Base.cpp
    src/Component/Group.cpp
    src/Component/Image.cpp
    src/Component/ImageMask.cpp
    src/Component/Rectangle.cpp
    src/Component/RectangleRound.cpp
    src/Component/Circle.cpp
    src/Component/Repeat.cpp
    src/Component/Svg.cpp
    src/Component/Text.cpp
    src/Component/TextArea.cpp
    src/Types/Color.cpp
    src/Renderer.cpp
)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
target_include_directories(${PROJECT_NAME} PUBLIC include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CAIRO_INCLUDE_DIRS})
link_directories(${PROJECT_NAME} PUBLIC ${CAIRO_LIBDIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${CAIRO_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${PANGO_INCLUDE_DIRS})
link_directories(${PROJECT_NAME} PUBLIC ${PANGO_LIBDIR})
link_directories(${PROJECT_NAME} PUBLIC ${PANGO_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PANGO_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${RSVG_INCLUDE_DIRS})
link_directories(${PROJECT_NAME} PUBLIC ${RSVG_LIBDIR})
link_directories(${PROJECT_NAME} PUBLIC ${RSVG_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${RSVG_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PUBLIC ${WEBP_INCLUDE_DIRS})
link_directories(${PROJECT_NAME} PUBLIC ${WEBP_LIBDIR})
target_link_libraries(${PROJECT_NAME} PUBLIC ${WEBP_LIBRARIES})

# Install
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
