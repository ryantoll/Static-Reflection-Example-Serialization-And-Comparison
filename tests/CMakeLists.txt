# CMakeList.txt : CMake project for Static-Reflection-Example-Serialization-And-Comparison, include source and define
# project specific logic here.
#

# Add source to this project's executable.

find_package(Catch2 3 REQUIRED)
add_executable (tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET tests PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
