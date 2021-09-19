cmake_minimum_required(VERSION 3.1)
project(Drcom8021X)
set(CMAKE_CXX_STANDARD 11)

find_package(PCAP REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIR}
)

file(GLOB SRC_FILE ./src/*)
list(FILTER SRC_FILE INCLUDE REGEX "[\\w]*.(cpp|c)")
list(FILTER SRC_FILE EXCLUDE REGEX "main.(cpp|c)")
add_library(${PROJECT_NAME} ${SRC_FILE})

add_executable(drcom ./src/main.cpp)

target_link_libraries(drcom
    ${PROJECT_NAME}
    ${PCAP_LIBRARY}
)