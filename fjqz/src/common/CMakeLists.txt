CollectSourceFiles(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_SOURCES
  )

GroupSources(${CMAKE_CURRENT_SOURCE_DIR})

# 制作图书馆
add_library(common
			${PRIVATE_SOURCES})

CollectIncludeDirectories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  PUBLIC_INCLUDES
  )

# target_include_directories将包含目录添加到目标。 PUBLIC对于可执行文件来说意义不大； 对于库，它让 CMake 知道链接到该目标的任何目标也必须需要该包含目录
target_include_directories(common
  PUBLIC
    # Provide the binary dir for all child targets为所有子目标提供二进制目录 
    ${CMAKE_BINARY_DIR}
    ${PUBLIC_INCLUDES}
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR})

# add_library(another STATIC another.cpp another.h)
# target_link_libraries(another PUBLIC one)
# target_link_libraries可能是 CMake 中最有用和最令人困惑的命令。 它需要一个目标（ another) 并在给定目标时添加依赖项。
# 如果没有该名称的目标（ one) 存在，然后它添加一个链接到一个名为的库 one在您的路径上（因此是命令的名称）
# 目标可以包含目录、链接库（或链接目标）、编译选项、编译定义、编译功能（参见 C++11 章节）等等
target_link_libraries(common
  PRIVATE
    fjqz-core-interface
  PUBLIC
    fmt
	json
    utf8cpp
    threads
    jemalloc
    gtest
    gmock)

# CMake 存储信息的另一种方式是在属性中。 这就像一个变量，但它附加到其他一些项目，如目录或目标。 全局属性可以是有用的未缓存全局变量。
# 许多目标属性是从匹配变量初始化的 CMAKE_在前面。 所以设置 CMAKE_CXX_STANDARD
# 例如，将意味着创建的所有新目标都将具有 CXX_STANDARD创建它们时设置为该值。 那里有两个 设置属性的方法：
#  set_property(TARGET TargetName
#             PROPERTY CXX_STANDARD 11)
#
# set_target_properties(TargetName PROPERTIES
#                      CXX_STANDARD 11)
# 第一种形式更通用，可以一次设置多个目标/文件/测试，并且有有用的选项。 第二个是在一个目标上设置多个属性的快捷方式。 您可以类似地获取属性：
# get_property(ResultVariable TARGET TargetName PROPERTY CXX_STANDARD)

set_target_properties(common
    PROPERTIES
      FOLDER
        "client"
        )
