# This file is part of the TrinityCore Project. See AUTHORS file for Copyright information
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

# User has manually chosen to ignore the git-tests, so throw them a warning.
# This is done EACH compile so they can be alerted about the consequences.

if(NOT BUILDDIR)
  # Workaround for funny MSVC behaviour - this segment is only used when using cmake gui
  set(BUILDDIR ${CMAKE_BINARY_DIR})
endif()

if(WITHOUT_GIT)
  set(rev_date "1970-01-01 00:00:00 +0000")
  set(rev_hash "unknown")
  set(rev_branch "Archived")
  # No valid git commit date, use today
  string(TIMESTAMP rev_date_fallback "%Y-%m-%d %H:%M:%S" UTC)
else()
  if(GIT_EXECUTABLE)
    # Retrieve repository dirty status检索存储库脏状态 
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" diff-index --quiet HEAD --
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      RESULT_VARIABLE is_dirty
    )

    # Create a revision-string that we can use创建一个我们可以使用的修订字符串
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" rev-parse --short=12 HEAD
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE rev_hash
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Append dirty marker to commit hash附加脏标记以提交哈希 
    if(is_dirty)
      set(rev_hash "${rev_hash}+")
    endif()

    # And grab the commits timestamp并获取提交时间戳 
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" show -s --format=%ci
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE rev_date
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # Also retrieve branch name还检索分支名称 
    execute_process(
      COMMAND "${GIT_EXECUTABLE}" symbolic-ref -q --short HEAD
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
      OUTPUT_VARIABLE rev_branch
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET
    )

    # when ran on CI, repository is put in detached HEAD state, attempt to scan for known local branches在 CI 上运行时，存储库处于分离的 HEAD 状态，尝试扫描已知的本地分支 
    if(NOT rev_branch)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" for-each-ref --points-at=HEAD refs/heads "--format=%(refname:short)"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE rev_branch
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )
    endif()

    # if local branch scan didn't find anything, try remote branches如果本地分支扫描没有找到任何东西，请尝试远程分支 
    if(NOT rev_branch)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" for-each-ref --points-at=HEAD refs/remotes "--format=%(refname:short)"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE rev_branch
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
      )
    endif()

    # give up finding a name for branch, use commit hash放弃寻找分支名称，使用提交哈希 
    if(NOT rev_branch)
      set(rev_branch ${rev_hash})
    endif()

    # normalize branch to single line (for-each-ref can output multiple lines if there are multiple branches on the same commit)将分支规范化为单行（如果同一个提交有多个分支，for-each-ref 可以输出多行） 
    string(REGEX MATCH "^[^ \t\r\n]+" rev_branch ${rev_branch})
  endif()

  # Last minute check - ensure that we have a proper revision
  # If everything above fails (means the user has erased the git revision control directory or removed the origin/HEAD tag)最后一分钟检查 - 确保我们有正确的修订 如果以上一切都失败（意味着用户已经删除了 git 修订控制目录或删除了 origin/HEAD 标签）
  if(NOT rev_hash)
    # No valid ways available to find/set the revision/hash, so let's force some defaults没有可用的有效方法来查找/设置修订版/散列，所以让我们强制使用一些默认值 
    message(STATUS "
    Could not find a proper repository signature (hash) - you may need to pull tags with git fetch -t
    Continuing anyway - note that the versionstring will be set to \"unknown 1970-01-01 00:00:00 (Archived)\"")
    set(rev_date "1970-01-01 00:00:00 +0000")
    set(rev_hash "unknown")
    set(rev_branch "Archived")
    # No valid git commit date, use today
    string(TIMESTAMP rev_date_fallback "%Y-%m-%d %H:%M:%S" UTC)
  else()
    # We have valid date from git commit, use that
    set(rev_date_fallback ${rev_date})
  endif()
endif()

# For package/copyright information we always need a proper date - keep "Archived/1970" for displaying git info but a valid year elsewhere对于包/版权信息，我们总是需要一个正确的日期 - 保留“Archived/1970”以显示 git 信息，但在其他地方保留有效年份 
string(REGEX MATCH "([0-9]+)-([0-9]+)-([0-9]+)" rev_date_fallback_match ${rev_date_fallback})
set(rev_year ${CMAKE_MATCH_1})
set(rev_month ${CMAKE_MATCH_2})
set(rev_day ${CMAKE_MATCH_3})

# Create the actual revision_data.h file from the above params从上述参数创建实际的 revision_data.h 文件
if(NOT "${rev_hash_cached}" STREQUAL "${rev_hash}" OR NOT "${rev_branch_cached}" STREQUAL "${rev_branch}" OR NOT EXISTS "${BUILDDIR}/revision_data.h")
  configure_file(
    "${CMAKE_SOURCE_DIR}/revision_data.h.in.cmake"
    "${BUILDDIR}/revision_data.h"
    @ONLY
  )
  set(rev_hash_cached "${rev_hash}" CACHE INTERNAL "Cached commit-hash")
  set(rev_branch_cached "${rev_branch}" CACHE INTERNAL "Cached branch name")
endif()
