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


add_executable( csv_rw                                csv_rw.cpp             )
add_executable( csv_data_vs_string                    csv_data_vs_string.cpp )

target_link_libraries( csv_rw                         ${DEFAULT_LIBRARIES}   )
target_link_libraries( csv_data_vs_string             ${DEFAULT_LIBRARIES}   )
