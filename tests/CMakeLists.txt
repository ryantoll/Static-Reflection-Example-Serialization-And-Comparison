﻿find_package(Catch2 3 REQUIRED)
add_executable (tests test.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/src)
