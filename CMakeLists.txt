cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(RTSP_SERVER_EXAMPLE)

add_compile_options(-fPIC)



if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")



set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(RTSP_SERVER_EXAMPLE
               main.cpp)


find_package(PkgConfig)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0)


pkg_search_module(gstreamer REQUIRED IMPORTED_TARGET gstreamer-1.0>=1.4)
pkg_search_module(gstreamer-sdp REQUIRED IMPORTED_TARGET gstreamer-sdp-1.0>=1.4)
pkg_search_module(gstreamer-app REQUIRED IMPORTED_TARGET gstreamer-app-1.0>=1.4)
pkg_search_module(gstreamer-video REQUIRED IMPORTED_TARGET gstreamer-video-1.0>=1.4)


include_directories(${GLIB_INCLUDE_DIRS}
                    ${GSTREAMER_INCLUDE_DIRS})

link_directories("/usr/lib/aarch64-linux-gnu")

link_directories(${GLIB_LIBRARY_DIRS}
                 ${GSTREAMER_LIBRARY_DIRS})



target_link_libraries(RTSP_SERVER_EXAMPLE
    PkgConfig::gstreamer
    PkgConfig::gstreamer-sdp
    PkgConfig::gstreamer-app
    PkgConfig::gstreamer-video
)


# must manually link these
target_link_libraries(RTSP_SERVER_EXAMPLE "/usr/lib/aarch64-linux-gnu/libgstvideo-1.0.so")
target_link_libraries(RTSP_SERVER_EXAMPLE "/usr/lib/aarch64-linux-gnu/libgstrtspserver-1.0.so")
target_link_libraries(RTSP_SERVER_EXAMPLE "/usr/lib/aarch64-linux-gnu/libgstbase-1.0.so")
target_link_libraries(RTSP_SERVER_EXAMPLE "/usr/lib/aarch64-linux-gnu/gstreamer-1.0/libgstrtsp.so")
target_link_libraries(RTSP_SERVER_EXAMPLE "/usr/lib/aarch64-linux-gnu/libgio-2.0.so")