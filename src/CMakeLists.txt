# CMakeList.txt : CMake project for Static-Reflection-Example-Serialization-And-Comparison, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (Static-Reflection-Example-Serialization-And-Comparison "main.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Static-Reflection-Example-Serialization-And-Comparison PROPERTY CXX_STANDARD 17)
endif()

# TODO: Add tests and install targets if needed.
