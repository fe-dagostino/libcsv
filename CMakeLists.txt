cmake_minimum_required(VERSION 3.16)
# Set the project name and language
project( csv
         VERSION 0.0.1
         DESCRIPTION "CSV parser."
         LANGUAGES CXX C
)

include_directories( ${CMAKE_SOURCE_DIR}/include         )
include_directories( ${CMAKE_SOURCE_DIR}/include/devices )

# Used as a tool to make sure that the standard used by tools
# and by the projects cannot get out of sync
set(USER_CXX_STANDARD 20)

# uncomment to set a default CXX standard for the external tools like clang-tidy and cppcheck
# and the targets that do not specify a standard.
# If not set, the latest supported standard for your compiler is used
# You can later set fine-grained standards for each target using `target_compile_features`
# Note: linking together projects compiled with different C++ standards may work, but
# it is not recommended because of possible issues with ABI
set(CMAKE_CXX_STANDARD ${USER_CXX_STANDARD})

# strongly encouraged to enable this globally to avoid conflicts between
# -Wpedantic being enabled and -std=c++20 and -std=gnu++20 for example
# when compiling with PCH enabled
set(CMAKE_CXX_EXTENSIONS ON)

IF(GENERATOR_IS_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
  # Make sure that all supported configuration types have their
  # associated conan packages available. You can reduce this
  # list to only the configuration types you use, but only if one
  # is not forced-set on the command line for VS
  set(CMAKE_CONFIGURATION_TYPES
      Debug
      Release
      RelWithDebInfo
      MinSizeRel)
ENDIF()


IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  set(  LIB_VERSION_SO        -${CMAKE_PROJECT_VERSION}-debug          )
  set(  LIB_VERSION_A         -${CMAKE_PROJECT_VERSION}-static-debug   )
ELSE()
  set(  LIB_VERSION_SO        -${CMAKE_PROJECT_VERSION}                )
  set(  LIB_VERSION_A         -${CMAKE_PROJECT_VERSION}-static         )
ENDIF()

set(  LIB_BINARY_DIR 
      ${CMAKE_CURRENT_BINARY_DIR}
   )

IF( CMAKE_BUILD_TYPE STREQUAL "Debug" )
  add_definitions(
                  -ggdb3
  )
ENDIF()

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

set(  LIB_INCLUDE

)

set(  LIB_SRC
  ./src/devices/csv_device.cpp
  ./src/devices/csv_dev_file.cpp
  ./src/csv_parser.cpp
  ./src/csv_base.cpp
  ./src/csv_reader.cpp
  ./src/csv_writer.cpp
)

add_library   ( csv${LIB_VERSION_SO}   SHARED ${LIB_SRC} )
add_library   ( csv${LIB_VERSION_A}    STATIC ${LIB_SRC} )

target_link_libraries( ${CMAKE_PROJECT_NAME}${LIB_VERSION_SO}  ${EXT_LIBRARIES} )
target_link_libraries( ${CMAKE_PROJECT_NAME}${LIB_VERSION_A}   ${EXT_LIBRARIES} )
target_link_libraries( ${CMAKE_PROJECT_NAME}${LIB_VERSION_SO}  ${CMAKE_DL_LIBS} )
target_link_libraries( ${CMAKE_PROJECT_NAME}${LIB_VERSION_A}   ${CMAKE_DL_LIBS} )


add_subdirectory(examples)
