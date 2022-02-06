# Build a list of all script modules when -DSCRIPT="custom" is selected

set(BUILD_SHARED_LIBS OFF)

option(WITH_WARNINGS    "Show all warnings during compile"                            1)
option(WITH_COREDEBUG   "Include additional debug-code in core"                       0)
option(COPY_CONF        "Copy authserver and worldserver .conf.dist files to the project dir"      0)
set(WITH_SOURCE_TREE    "hierarchical" CACHE STRING "Build the source tree for IDE's.")
set_property(CACHE WITH_SOURCE_TREE PROPERTY STRINGS no flat hierarchical hierarchical-folders)
option(BUILD_TESTING    "Build test suite" 0)

if(UNIX)
  option(USE_LD_GOLD    "Use GNU gold linker"                                        0)
endif()
