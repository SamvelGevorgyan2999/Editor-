# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "AdvancedTextEditor_autogen"
  "CMakeFiles/AdvancedTextEditor_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/AdvancedTextEditor_autogen.dir/ParseCache.txt"
  "CMakeFiles/ate_core_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/ate_core_autogen.dir/ParseCache.txt"
  "ate_core_autogen"
  "tests/CMakeFiles/test_documentstats_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/test_documentstats_autogen.dir/ParseCache.txt"
  "tests/CMakeFiles/test_filemanager_autogen.dir/AutogenUsed.txt"
  "tests/CMakeFiles/test_filemanager_autogen.dir/ParseCache.txt"
  "tests/test_documentstats_autogen"
  "tests/test_filemanager_autogen"
  )
endif()
