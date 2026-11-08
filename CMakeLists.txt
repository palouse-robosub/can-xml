cmake_minimum_required(VERSION 3.13)
set(CMAKE_CXX_STANDARD 17)
project(can_xml)

include(FetchContent)
# Getting tinyXML
FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 9148bdf719e997d1f474be6bcc7943881046dba1
)
FetchContent_MakeAvailable(tinyxml2)

set(CAN_XML ${CMAKE_CURRENT_BINARY_DIR}/generated/guppy_can.hpp)

add_custom_command(
    OUTPUT ${CAN_XML}
    COMMAND can_generator
    DEPENDS can_generator # codegen
    COMMENT "Generating guppy_can.hpp"
)
add_custom_target(run_can_generator ALL DEPENDS ${CAN_XML})

# Inserting shared.hpp and can-ids.xml directly into can_generator.cpp
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/resource/can-ids.xml CAN_IDS_XML)
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/include/shared.hpp SHARED_CAN_CODE)
configure_file(src/can_generator.cpp can_generator_replaced.cpp @ONLY)

add_executable(can_generator can_generator_replaced.cpp include/shared.hpp)
target_link_libraries(can_generator tinyxml2)
target_include_directories(can_generator PRIVATE include)

add_library(guppy_codegen INTERFACE)
target_include_directories(guppy_codegen INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/generated)

