cmake_minimum_required(VERSION 3.16)
# Set the project name and language
project( tests
         LANGUAGES CXX C
)

# Add the temporary output directories to the library path to make sure that
# libcsv can be found, even if it is not installed system-wide yet.
link_directories( ${LIB_BINARY_DIR} )

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

#find_package(GTest REQUIRED)

set(  DEFAULT_LIBRARIES
      csv${LIB_VERSION_SO}
      gtest_main
)

enable_testing()

#add_executable( csv_device_file_test                  csv_device_file_test.cpp  )
#add_executable( csv_data_test                         csv_data_test.cpp         )

#target_link_libraries( csv_device_file_test           ${DEFAULT_LIBRARIES}   )
#target_link_libraries( csv_data_test                  ${DEFAULT_LIBRARIES}   )

include(GoogleTest)
#gtest_discover_tests(csv_device_file_test)
#gtest_discover_tests(csv_data_test)