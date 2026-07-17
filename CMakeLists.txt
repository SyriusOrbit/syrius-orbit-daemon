cmake_minimum_required(VERSION 3.20)

project(syrius-orbit-daemon
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(mosquitto)
find_package(plog)

include_directories(include)
add_library(syrius_orbit_daemon STATIC src/daemon.cpp)
target_link_libraries(syrius_orbit_daemon PRIVATE mosquitto::mosquitto plog::plog)

if(MSVC)
    target_compile_options(syrius_orbit_daemon PRIVATE /W4 /WX)
else()
    target_compile_options(syrius_orbit_daemon PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_executable(${PROJECT_NAME} src/main.cpp )
target_link_libraries(${PROJECT_NAME} PRIVATE syrius_orbit_daemon mosquitto::mosquitto plog::plog)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
