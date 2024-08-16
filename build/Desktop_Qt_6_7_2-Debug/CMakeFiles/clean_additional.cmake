# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/appJUCETest_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/appJUCETest_autogen.dir/ParseCache.txt"
  "appJUCETest_autogen"
  )
endif()
