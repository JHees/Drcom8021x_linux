cmake_minimum_required(VERSION 3.4)
project(Drcom8021X)

include(GNUInstallDirs)

set(CMAKE_CXX_STANDARD 11)

find_package(PCAP REQUIRED)
find_package(Boost REQUIRED)
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PCAP_INCLUDE_DIR}
)

file(GLOB SRC_FILE ./src/*)
list(FILTER SRC_FILE INCLUDE REGEX "[\\w]*.(cpp|c)")
list(FILTER SRC_FILE EXCLUDE REGEX "main.(cpp|c)")
add_library(${PROJECT_NAME} ${SRC_FILE})

add_executable(drcom8021x ./src/main.cpp)

target_link_libraries(drcom8021x
    ${PROJECT_NAME}
    ${PCAP_LIBRARY}
)

# install target
# debug install
install(TARGETS drcom8021x
    CONFIGURATIONS Debug
    RUNTIME DESTINATION ${PROJECT_SOURCE_DIR}/debug/bin
    LIBRARY DESTINATION ${PROJECT_SOURCE_DIR}/debug/lib
    ARCHIVE DESTINATION ${PROJECT_SOURCE_DIR}/debug/lib
)
install(FILES drcom8021x.service drcom8021x.timer
    CONFIGURATIONS Debug
    DESTINATION ${PROJECT_SOURCE_DIR}/debug/service

)
install(FILES drcom8021x.conf
    CONFIGURATIONS Debug
    DESTINATION ${PROJECT_SOURCE_DIR}/debug/conf
)

install(TARGETS drcom8021x
    CONFIGURATIONS Release
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# release install
install(FILES drcom8021x.service drcom8021x.timer
    CONFIGURATIONS Release
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/systemd/system
)
install(FILES drcom8021x.conf
    CONFIGURATIONS Release
    DESTINATION /etc
)


# uninstall target
if(NOT TARGET uninstall)
  configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)
  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()
