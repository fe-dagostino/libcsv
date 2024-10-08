cmake_minimum_required(VERSION 3.22)

set(libname "csv")

# Set the project name and language
project( ${libname}
         VERSION 1.2.0
         DESCRIPTION "CSV parser."
         LANGUAGES CXX C
)

configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include                        )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/devices                )

set(CMAKE_MODULE_PATH   ${PROJECT_SOURCE_DIR}/cmake )

# uncomment to set a default CXX standard for the external tools like clang-tidy and cppcheck
# and the targets that do not specify a standard.
# If not set, the latest supported standard for your compiler is used
# You can later set fine-grained standards for each target using `target_compile_features`
# Note: linking together projects compiled with different C++ standards may work, but
# it is not recommended because of possible issues with ABI
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS ON)

if(GENERATOR_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
  # Make sure that all supported configuration types have their
  # associated conan packages available. You can reduce this
  # list to only the configuration types you use, but only if one
  # is not forced-set on the command line for VS
  set(CMAKE_CONFIGURATION_TYPES
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
endif()


if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(  LIB_VERSION_SO        -${PROJECT_VERSION}-debug          )
  set(  LIB_VERSION_A         -${PROJECT_VERSION}-static-debug   )
else()
  set(  LIB_VERSION_SO        -${PROJECT_VERSION}                )
  set(  LIB_VERSION_A         -${PROJECT_VERSION}-static         )
endif()

set(  LIB_BINARY_DIR 
      ${CMAKE_CURRENT_BINARY_DIR}
   )

if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_definitions(
                  -ggdb3
  )
endif()

if(MSVC)
  # warning level 4 and all warnings as errors
  add_compile_options(-D_CRT_SECURE_NO_WARNINGS /W4 /WX)
else()
  # lots of warning
  add_compile_options(-Wall -Wextra -pedantic)
  set(  EXT_LIBRARIES
      m
      pthread
  )
endif()

# Fetch lock-free library and make it available as dependency
include(FetchContent)
include(lock-free)
list( APPEND EXT_LIBRARIES lock-free )

option(CSV_BUILD_EXAMPLES "Enable/Disable examples build"  ON)
option(CSV_BUILD_TESTS    "Enable/Disable tests build"    OFF)

file( GLOB_RECURSE 
      LIB_INCLUDE
      ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    )

file( GLOB_RECURSE 
      LIB_SRC
      ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    )

add_library   ( ${libname}${LIB_VERSION_SO}   SHARED ${LIB_SRC} )
add_library   ( ${libname}${LIB_VERSION_A}    STATIC ${LIB_SRC} )
add_library( libcsv::libcsv        ALIAS ${libname}${LIB_VERSION_SO} )
add_library( libcsv::libcsv_static ALIAS ${libname}${LIB_VERSION_A}  )

target_link_libraries( ${libname}${LIB_VERSION_SO}  ${EXT_LIBRARIES} )
target_link_libraries( ${libname}${LIB_VERSION_A}   ${EXT_LIBRARIES} )
target_link_libraries( ${libname}${LIB_VERSION_SO}  ${CMAKE_DL_LIBS} )
target_link_libraries( ${libname}${LIB_VERSION_A}   ${CMAKE_DL_LIBS} )

target_include_directories( ${libname}${LIB_VERSION_SO} INTERFACE 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include/devices" 
                          )
target_include_directories( ${libname}${LIB_VERSION_A}  INTERFACE 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include" 
                            "${CMAKE_CURRENT_SOURCE_DIR}/include/devices" 
                          )

include(GNUInstallDirs)
install( TARGETS ${libname}${LIB_VERSION_SO} ARCHIVE DESTINATION lib )
install( TARGETS ${libname}${LIB_VERSION_A}  ARCHIVE DESTINATION lib )

install( FILES   ${LIB_INCLUDE} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} ) 


if ( CSV_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

if ( CSV_BUILD_TESTS )
  add_subdirectory(tests)
endif()
