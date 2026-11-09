cmake_minimum_required(VERSION 3.13)
project(can_xml LANGUAGES CXX)

include(FetchContent)

find_package(tinyxml2 QUIET)
if(NOT tinyxml2_FOUND)
  FetchContent_Declare(
    tinyxml2
    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
    GIT_TAG 9148bdf719e997d1f474be6bcc7943881046dba1
  )
  FetchContent_MakeAvailable(tinyxml2)
endif()

if(TARGET tinyxml2::tinyxml2)
  set(TINYXML2_TARGET tinyxml2::tinyxml2)
else()
  set(TINYXML2_TARGET tinyxml2)
endif()

# public library for guppy ros

add_library(${PROJECT_NAME} INTERFACE)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)
target_include_directories(${PROJECT_NAME} INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(${PROJECT_NAME} INTERFACE
  ${TINYXML2_TARGET}
)

# optional code generation
option(CAN_XML_GENERATE
  "generate can header from xml"
  OFF
)

if(CAN_XML_GENERATE)
  # configure generating executable

  set(CAN_XML ${CMAKE_CURRENT_BINARY_DIR}/generated/guppy_can.hpp)

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)
  # Inserting shared.hpp and can-ids.xml directly into can_generator.cpp
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/resource/can-ids.xml CAN_IDS_XML)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/resource/before.hpp BEFORE_CODE)
  configure_file(src/can_generator.cpp can_generator_replaced.cpp @ONLY)

  add_executable(can_generator
    ${CMAKE_CURRENT_BINARY_DIR}/can_generator_replaced.cpp
  )
  target_link_libraries(can_generator
    PRIVATE
      ${PROJECT_NAME}
  )
  target_include_directories(can_generator
    PRIVATE
      ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
  target_compile_features(can_generator PRIVATE cxx_std_20)
  target_compile_options(can_generator PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall -Wextra -Wpedantic>
  )

  # generate header

  add_custom_command(
      OUTPUT ${CAN_XML}
      COMMAND $<TARGET_FILE:can_generator>
      DEPENDS can_generator
      COMMENT "Generating guppy_can.hpp"
      VERBATIM
  )
  add_custom_target(run_can_generator ALL
    DEPENDS ${CAN_XML}
  )

  # generated header library for guppy embedded

  add_library(guppy_codegen INTERFACE)
  target_include_directories(guppy_codegen INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
  )
  add_dependencies(guppy_codegen run_can_generator)
endif()
