cmake_minimum_required(VERSION 3.20)

project(syrius-orbit-daemon
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(mosquitto)
find_package(plog)
add_compile_definitions(PLOG_CAPTURE_FILE)
find_package(httplib REQUIRED)
find_package(argparse REQUIRED)
find_package(mosquitto REQUIRED COMPONENTS libmosquittopp)
find_package(SQLiteCpp REQUIRED)

include_directories(include)
include(cmake/migrations.cmake)

add_executable(
    ${PROJECT_NAME}
    src/main.cpp
    src/Daemon.cpp
    src/SpatialService.cpp
    src/FleetGateway.cpp
    src/VDA5050Proxy.cpp
    src/SchemaMigrator.cpp
    src/db/Database.cpp
    src/db/Vda5050EventsRepository.cpp
    src/db/RobotsRepository.cpp
    src/db/OrdersRepository.cpp
    src/db/InstantActionsRepository.cpp
    src/db/MapsRepository.cpp
    ${MIGRATION_GENERATED_SOURCES}
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    mosquitto::mosquitto
    plog::plog
    httplib::httplib
    argparse::argparse
    SQLiteCpp
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

