add_subdirectory(threads)

if(SERVERS OR TOOLS)
  add_subdirectory(boost)
  add_subdirectory(zlib)
  # add_subdirectory(g3dlite)
  # add_subdirectory(recastnavigation)
  add_subdirectory(fmt)
  add_subdirectory(SFMT) # 随机
  add_subdirectory(utf8cpp)
  add_subdirectory(valgrind) #内存检查
  add_subdirectory(openssl)
  add_subdirectory(jemalloc)
  add_subdirectory(argon2) # 哈希
endif()

if(SERVERS)
  add_subdirectory(mysql)
  add_subdirectory(readline)
  # add_subdirectory(gsoap)
  add_subdirectory(efsw) # 加密
endif()

if(TOOLS)
  add_subdirectory(bzip2)
  add_subdirectory(libmpq)
endif()

if(BUILD_TESTING)
  add_subdirectory(gtest)
  add_subdirectory(gmock)
endif()
