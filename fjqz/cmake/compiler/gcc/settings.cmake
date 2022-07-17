# Set build-directive (used in core to tell which buildtype we used)
target_compile_definitions(fjqz-compile-option-interface
  INTERFACE
    -D_BUILD_DIRECTIVE="$<CONFIG>")
    
LOG_DEBUG("gcc CONFIG = ${CONFIG}; INTERFACE = ${INTERFACE}")

set(GCC_EXPECTED_VERSION 8.3.0)

if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS GCC_EXPECTED_VERSION)
  message(FATAL_ERROR "GCC: TrinityCore requires version ${GCC_EXPECTED_VERSION} to build but found ${CMAKE_CXX_COMPILER_VERSION}")
else()
  message(STATUS "GCC: Minimum version required is ${GCC_EXPECTED_VERSION}, found ${CMAKE_CXX_COMPILER_VERSION} - ok!")
endif()

if(PLATFORM EQUAL 32)
  # Required on 32-bit systems to enable SSE2 (standard on x64)
  target_compile_options(fjqz-compile-option-interface
    INTERFACE
      -msse2
      -mfpmath=sse)
endif()
if(NOT CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
  target_compile_definitions(fjqz-compile-option-interface
    INTERFACE
      -DHAVE_SSE2
      -D__SSE2__)
  message(STATUS "GCC: SFMT enabled, SSE2 flags forced")
endif()

if(WITH_WARNINGS)
  target_compile_options(fjqz-warning-interface
    INTERFACE
      -W
      -Wall
      -Wextra
      -Winit-self
      -Winvalid-pch
      -Wfatal-errors
      -Woverloaded-virtual)

  message(STATUS "GCC: All warnings enabled")
endif()

if(WITH_COREDEBUG)
  target_compile_options(fjqz-compile-option-interface
    INTERFACE
      -g3)

  message(STATUS "GCC: Debug-flags set (-g3)")
endif()

# Sanitizers(请参考https://github.com/google/Sanitizers )已经成为静态和动态代码分析的非常有用的工具。
# 通过使用适当的标志重新编译代码并链接到必要的库，可以检查内存错误(地址清理器)、未初始化的读取(内存清理器)、
# 线程安全(线程清理器)和未定义的行为(未定义的行为清理器)相关的问题。与同类型分析工具相比，Sanitizers带来的性能损失通常要小得多，
# 而且往往提供关于检测到的问题的更详细的信息。缺点是，代码(可能还有工具链的一部分)需要使用附加的标志重新编译。
if(ASAN)
  target_compile_options(fjqz-compile-option-interface
    INTERFACE
      -fno-omit-frame-pointer
      -fsanitize=address
      -fsanitize-recover=address
      -fsanitize-address-use-after-scope)

  target_link_options(fjqz-compile-option-interface
    INTERFACE
      -fno-omit-frame-pointer
      -fsanitize=address
      -fsanitize-recover=address
      -fsanitize-address-use-after-scope)

  message(STATUS "GCC: Enabled Address Sanitizer")
endif()

if(BUILD_SHARED_LIBS)
  target_compile_options(fjqz-compile-option-interface
    INTERFACE
      -fPIC
      -Wno-attributes)

  target_compile_options(fjqz-hidden-symbols-interface
    INTERFACE
      -fvisibility=hidden)

  # Should break the build when there are TRINITY_*_API macros missing
  # but it complains about missing references in precompiled headers.
  # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,--no-undefined")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--no-undefined")

  message(STATUS "GCC: Enabled shared linking")
endif()
