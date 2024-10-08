cmake_minimum_required(VERSION 3.16)
# Set the project name and language
project( examples
         LANGUAGES CXX C
)

# Add the temporary output directories to the library path to make sure that
# libcsv can be found, even if it is not installed system-wide yet.
LINK_DIRECTORIES( ${LIB_BINARY_DIR} )

set(  DEFAULT_LIBRARIES
      csv${LIB_VERSION_SO}
)

option(CSV_ENABLE_CPPCHECK    "Enable/Disable cppcheck"    OFF)

if ( CSV_ENABLE_CPPCHECK )
  set(CMAKE_CXX_CPPCHECK "cppcheck")
endif()

add_executable( csv_rw                                csv_rw.cpp             )
add_executable( csv_data_vs_string                    csv_data_vs_string.cpp )
add_executable( csv_stress_memory                     csv_stress_memory.cpp  )

target_link_libraries( csv_rw                         ${DEFAULT_LIBRARIES}   )
target_link_libraries( csv_data_vs_string             ${DEFAULT_LIBRARIES}   )
target_link_libraries( csv_stress_memory              ${DEFAULT_LIBRARIES}   )
