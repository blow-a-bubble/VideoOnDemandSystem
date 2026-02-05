#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "scaffold::scaffold" for configuration ""
set_property(TARGET scaffold::scaffold APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(scaffold::scaffold PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libscaffold.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS scaffold::scaffold )
list(APPEND _IMPORT_CHECK_FILES_FOR_scaffold::scaffold "${_IMPORT_PREFIX}/lib/libscaffold.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
