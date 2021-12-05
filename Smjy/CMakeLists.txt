cmake_minimum_required(VERSION 3.11)

# add this options before PROJECT keyword // ？？禁止避免CMakeFiles/并CMakeCache.txt在源目录中创建
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Smjy VERSION 0.01)

# CMake policies (can not be handled elsewhere) // 升级兼容低版本告警
cmake_policy(SET CMP0005 NEW)
if(CMAKE_VERSION VERSION_LESS "3.16.0")
  cmake_policy(SET CMP0043 OLD) # Disable 'Ignore COMPILE_DEFINITIONS_<Config> properties'
else()
  cmake_policy(SET CMP0043 NEW) # Cotire isn't used so set to NEW
endif()
cmake_policy(SET CMP0054 NEW) # Only interpret if() arguments as variables or keywords when unquoted - prevents intepreting if(SOME_STRING_VARIABLE MATCHES "MSVC") as if(SOME_STRING_VARIABLE MATCHES "1")

if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW) # find_package() uses <PackageName>_ROOT variables
endif()

# Set RPATH-handing (CMake parameters)  rpath全称是run-time search path 当动态库的编译也和执行档在同级目录下的时候，CMake会自动给执行档加入适当的RPATH 相关顺序这里简单交代下： RPATH   --   LD_LIBRARY_PATH -- RUNPATH(出现会屏蔽RPATH) -- /etc/ld.so.conf -- builtin dircetories(/lib, /usr/lib)。
set(CMAKE_SKIP_BUILD_RPATH 0)
set(CMAKE_BUILD_WITH_INSTALL_RPATH 0)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH 1)

# set macro-directory  // 添加模块路径
list(APPEND CMAKE_MODULE_PATH
  "${CMAKE_SOURCE_DIR}/cmake/macros")

if(CMAKE_VERSION VERSION_LESS "3.16.0")
  list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/dep/cotire/CMake")
endif()

# build in Release-mode by default if not explicitly set // 根据环境生成makefile
if(CMAKE_GENERATOR STREQUAL "Ninja Multi-Config")
  set(CMAKE_DEFAULT_BUILD_TYPE "RelWithDebInfo" CACHE INTERNAL "")
endif()
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "RelWithDebInfo")
endif()

#  include 指令用来载入并运行来自于文件或模块的 CMake 代码。
include(Debug)
include(CheckCXXSourceRuns)
include(CheckIncludeFiles)
include(ConfigureScripts)

# set default buildoptions and print them
include(cmake/options.cmake)

# turn off PCH totally if enabled (hidden setting, mainly for devs)
if(NOPCH)
  set(USE_COREPCH 0)
  set(USE_SCRIPTPCH 0)
endif()

# 添加编译器标准和库接口
include(ConfigureBaseTargets)
# 检查平台
include(CheckPlatform)

include(GroupSources)
include(AutoCollect)

# include(cmake/external_module.cmake)

# find_package 查找模块
find_package(PCHSupport)
find_package(MySQL)
find_package(GTest)
find_package(GMock)

if(NOT WITHOUT_GIT)
  find_package(Git 1.7)
endif()

# Find revision ID and hash of the sourcetree 查找源树的修订 ID 和哈希 
include(cmake/genrev.cmake)

# print out the results before continuing 在继续之前打印出结果 
include(cmake/showoptions.cmake)

# 添加第三方库目录
add_subdirectory(externals)

# 添加源码目录
add_subdirectory(src)

# 配置项目以使用 CTest/CDash 进行测试
include(CTest)
if(BUILD_TESTING)
  list(APPEND CMAKE_MODULE_PATH
    "${Catch2_SOURCE_DIR}/contrib")
  include(Catch)

  add_subdirectory(tests)

  # Catch cmakefile messes with our settings we explicitly leave up to the user
  # restore user preference
  if (NOT WITH_SOURCE_TREE STREQUAL "hierarchical-folders")
    set_property(GLOBAL PROPERTY USE_FOLDERS OFF)
  endif()
endif()
