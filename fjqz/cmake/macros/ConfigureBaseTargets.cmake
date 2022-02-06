# An interface library to make the target com available to other targets 使目标 com 可用于其他目标的接口库 
add_library(fjqz-compile-option-interface INTERFACE)

# Use -std=c++11 instead of -std=gnu++11
set(CXX_EXTENSIONS ON)

# An interface library to make the target features available to other targets 使目标功能可用于其他目标的接口库 
add_library(fjqz-feature-interface INTERFACE)

target_compile_features(fjqz-feature-interface
  INTERFACE
    cxx_std_17)

# An interface library to make the warnings level available to other targets
# This interface taget is set-up through the platform specific script 使警告级别可用于其他目标的接口库。 此接口目标是通过平台特定脚本设置的 
add_library(fjqz-warning-interface INTERFACE)

# An interface used for all other interfaces 用于所有其他接口的接口 
add_library(fjqz-default-interface INTERFACE)
target_link_libraries(fjqz-default-interface
  INTERFACE
    fjqz-compile-option-interface
    fjqz-feature-interface)

# An interface used for silencing all warnings 用于消除所有警告的接口 
add_library(fjqz-no-warning-interface INTERFACE)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  target_compile_options(fjqz-no-warning-interface
    INTERFACE
      /W0)
else()
  target_compile_options(fjqz-no-warning-interface
    INTERFACE
      -w)
endif()

# An interface library to change the default behaviour 一个界面库，用于更改默认行为以自动隐藏符号。
# to hide symbols automatically.
add_library(fjqz-hidden-symbols-interface INTERFACE)

# An interface amalgamation which provides the flags and definitions
# used by the dependency targets. 提供依赖目标使用的标志和定义的接口合并。 
add_library(fjqz-externals-interface INTERFACE)
target_link_libraries(fjqz-externals-interface
  INTERFACE
    fjqz-default-interface
    fjqz-no-warning-interface
    fjqz-hidden-symbols-interface)

# An interface amalgamation which provides the flags and definitions
# used by the core targets. 提供核心目标使用的标志和定义的接口合并。 
add_library(fjqz-core-interface INTERFACE)
target_link_libraries(fjqz-core-interface
  INTERFACE
    fjqz-default-interface
    fjqz-warning-interface)
