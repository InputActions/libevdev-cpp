find_package(PkgConfig REQUIRED)
pkg_search_module(LIBEVDEV REQUIRED libevdev)

add_library(libevdev-cpp STATIC
    libevdev-cpp/LibevdevDevice.cpp
    libevdev-cpp/LibevdevUinputDevice.cpp
    libevdev-cpp/logging.cpp
)
target_link_libraries(libevdev-cpp PUBLIC
    Qt6::Core
    ${LIBEVDEV_LIBRARIES}
)
target_compile_options(libevdev-cpp PUBLIC -fPIC)

target_include_directories(libevdev-cpp PRIVATE ${LIBEVDEV_INCLUDE_DIRS})
target_include_directories(libevdev-cpp PUBLIC libevdev-cpp)
