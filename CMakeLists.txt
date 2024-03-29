cmake_minimum_required(VERSION 3.16)

set(libname "csv")

# Set the project name and language
project( ${libname}
         VERSION 1.2.0
         DESCRIPTION "CSV parser."
         LANGUAGES CXX C
)

find_package(Git)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
# Update submodules as needed
    option(GIT_SUBMODULE "Update submodules with remote" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote -f
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init --recursive --remote -f : failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/lock-free/CMakeLists.txt")
    message(FATAL_ERROR "The submodules/lock-free were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
endif()

add_subdirectory(submodules/lock-free)


configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/submodules/lock-free/include/  )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include                        )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include/devices                )


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

if ( CSV_BUILD_EXAMPLES )
  add_subdirectory(examples)
endif()

if ( CSV_BUILD_TESTS )
  add_subdirectory(tests)
endif()
