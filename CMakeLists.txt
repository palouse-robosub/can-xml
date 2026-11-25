cmake_minimum_required(VERSION 3.23)
project(can_xml LANGUAGES CXX)


#if(NOT tinyxml2_FOUND)
#  include(FetchContent)
#
#  FetchContent_Declare(
#    tinyxml2
#    GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
#    GIT_TAG 9148bdf719e997d1f474be6bcc7943881046dba1
#  )
#  FetchContent_MakeAvailable(tinyxml2)
#endif()

find_package(tinyxml2 CONFIG QUIET)
if(NOT tinyxml2_FOUND)
  find_package(PkgConfig REQUIRED)
  pkg_check_modules(TINYXML2 REQUIRED IMPORTED_TARGET GLOBAL tinyxml2)
  add_library(tinyxml2::tinyxml2 ALIAS PkgConfig::TINYXML2)
endif()

find_package(ament_cmake QUIET)
if (ament_cmake_FOUND)
  set(CAN_XML_LIBDIR lib)
  set(CAN_XML_INCLUDEDIR include)
  set(CAN_XML_BINDIR bin)
  set(CAN_XML_DATADIR share)
else()
  include(GNUInstallDirs)
  set(CAN_XML_LIBDIR ${CMAKE_INSTALL_LIBDIR})
  set(CAN_XML_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR})
  set(CAN_XML_BINDIR ${CMAKE_INSTALL_BINDIR})
  set(CAN_XML_DATADIR ${CMAKE_INSTALL_BINDIR})
endif()

# public library for guppy ros

add_library(${PROJECT_NAME} SHARED
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xml_parse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/xml_parse_extern.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CAN_XML_INCLUDEDIR}>
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  $<BUILD_INTERFACE:tinyxml2::tinyxml2>
)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}Targets
  ARCHIVE DESTINATION ${CAN_XML_LIBDIR}
  LIBRARY DESTINATION ${CAN_XML_LIBDIR}
  RUNTIME DESTINATION ${CAN_XML_BINDIR}
)
install(FILES
  include/can_xml/common.hpp
  include/can_xml/xml_parse_extern.hpp
  DESTINATION ${CAN_XML_INCLUDEDIR}/can_xml
)

# optional code generation
option(CAN_XML_GENERATE "generate can header from xml" ON)

if(CAN_XML_GENERATE)
  # configure generating executable

  set(CAN_XML ${CMAKE_CURRENT_BINARY_DIR}/generated/guppy_can.hpp)
  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/can-ids.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/resource/prelude.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/can_generator.cpp
  )
  # Inserting shared.hpp and can-ids.xml directly into can_generator.cpp
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/resource/can-ids.xml CAN_IDS_XML)
  file(READ ${CMAKE_CURRENT_SOURCE_DIR}/resource/prelude.hpp CAN_HEADER_PRELUDE)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/can_generator.cpp can_generator_replaced.cpp @ONLY)

  add_executable(can_generator
    ${CMAKE_CURRENT_BINARY_DIR}/can_generator_replaced.cpp
  )
  target_compile_options(can_generator PRIVATE
    $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall;-Wextra;-Wpedantic>
  )
  target_link_libraries(can_generator PRIVATE
    ${PROJECT_NAME}
    tinyxml2::tinyxml2
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

# Added for IDE intellisense
add_executable(can_generator_ide EXCLUDE_FROM_ALL
  ${CMAKE_CURRENT_SOURCE_DIR}/src/can_generator.cpp
)
target_link_libraries(can_generator_ide
  PRIVATE
    ${PROJECT_NAME}
    tinyxml2::tinyxml2
)

if(ament_cmake_FOUND)
  ament_index_register_resource(can_ids
    CONTENT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/resource/can-ids.xml
  )
  ament_export_targets(${PROJECT_NAME}Targets HAS_LIBRARY_TARGET)
  ament_package()
else()
  install(EXPORT ${PROJECT_NAME}Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CAN_XML_DATADIR}/${PROJECT_NAME}/cmake
  )
endif()
