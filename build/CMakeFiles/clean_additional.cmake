# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/SistemaMatricula_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/SistemaMatricula_autogen.dir/ParseCache.txt"
  "SistemaMatricula_autogen"
  )
endif()
