# This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

macro(GroupSources dir)
  # Skip this if WITH_SOURCE_TREE is not set (empty string).
  if(NOT ${WITH_SOURCE_TREE} STREQUAL "")
    # Include all header and c files
    file(GLOB_RECURSE elements RELATIVE ${dir} *.h *.hpp *.c *.cpp *.cc)

    foreach(element ${elements})
      # Extract filename and directory
      get_filename_component(element_name ${element} NAME)
      get_filename_component(element_dir ${element} DIRECTORY)

      if(NOT ${element_dir} STREQUAL "")
        # If the file is in a subdirectory use it as source group.
        if(${WITH_SOURCE_TREE} STREQUAL "flat")
          # Build flat structure by using only the first subdirectory.
          string(FIND ${element_dir} "/" delemiter_pos)
          if(NOT ${delemiter_pos} EQUAL -1)
            string(SUBSTRING ${element_dir} 0 ${delemiter_pos} group_name)
            # 您可以使用正则表达式或显式列表控制文件在每个文件夹中的显示方式source_group：
            # 您还可以控制目标内的文件夹的显示方式。有两种方法，都使用source_group命令。您可以使用 明确列出文件FILES，
            # 或使用REGULAR_EXPRESSION. 这样您就可以完全控制文件夹结构。但是，如果您的磁盘布局设计得很好，您可能只想模仿它。
            # 在 CMake 3.8+ 中，您可以使用新版本的source_group命令轻松完成此操作：
            # source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/base/dir" PREFIX "Header Files" FILES ${FILE_LIST})
            source_group("${group_name}" FILES ${dir}/${element})
          else()
            # Build hierarchical structure.
            # File is in root directory.
            source_group("${element_dir}" FILES ${dir}/${element})
          endif()
        else()
          # Use the full hierarchical structure to build source_groups.
          string(REPLACE "/" "\\" group_name ${element_dir})
          source_group("${group_name}" FILES ${dir}/${element})
        endif()
      else()
        # If the file is in the root directory, place it in the root source_group.
        source_group("\\" FILES ${dir}/${element})
      endif()
    endforeach()
  endif()
endmacro()

if(WITH_SOURCE_TREE STREQUAL "hierarchical-folders")
  # Use folders
  # 一些 IDE，比如 Xcode，支持文件夹。您必须手动启用USE_FOLDERS全局属性以允许 CMake 按文件夹组织文件：
  # set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  # 然后，您可以在创建目标后将目标添加到文件夹： 文件夹可以嵌套/
  # set_property(TARGET MyFile PROPERTY FOLDER "Scripts")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()
