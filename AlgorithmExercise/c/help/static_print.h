/*
 * Copyright (C) 2019
 */

#ifndef TMP_STATIC_PRINT
#define TMP_STATIC_PRINT

#define JOIN_ITEMS(symbol1, symbol2) symbol1##symbol2 // 连接字符串
#define JOIN(symbol1, symbol2) JOIN_ITEMS(symbol1, symbol2) 
#define UNIQUE_NAME(prefix) JOIN(prefix, __LINE__)
#define PRINT(...) (char UNIQUE_NAME(print_value) = Print<__VA_ARGS__>())

#endif
