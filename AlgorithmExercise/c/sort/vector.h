/*
 * Copyright (C) 2020
 */

#ifndef _DYNAMIC_VECTOR_H
#define _DYNAMIC_VECTOR_H

#include <assert.h>
#include <stdlib.h>

#define vector_type(type) type*

// 设置向量的容量
#define set_vector_capacity(vec, size)		\
	do{										\
		if(vec)								\
			((size_t*)(vec))[-1] = (size);	\
	}while(0)


// 设置向量的大小
#define set_vector_size(vec, size)			\
	do{										\
		if(vec)								\
			((size_t*)(vec))[-2] = (size);	\
	}while(0)

// 获取当前的vector容量
#define get_vector_capacity(vec)	\
	((vec) ? ((size_t*)(vec))[-1] : (size_t)0)

// 获取向量的当前大小
#define get_vector_size(vec)	\
	((vec) ? ((size_t*)(vec))[-2] : (size_t)0)

#define vector_is_empty(vec)	\
	(get_vector_size(vec) == 0)

// 确保向量至少count个元素大小
#define vector_grow(vec, count)	\
	do{	\
		const size_t bytes = (count) * sizeof(*(vec)) + (sizeof(size_t) * 2); \
		if(!(vec))	\
		{	\
			size_t* vec_ptr = malloc(bytes);	\
			assert(vec_ptr);					\
			(vec) = (void *)(&vec_ptr[2]);		\
			set_vector_capacity((vec), (count));	\
			set_vector_size((vec), 0);				\
		}											\
		else										\
		{											\
			size_t* vec_ptr = &((size_t*)(vec))[-2];	\
			size_t* new_vec_ptr = realloc(vec_ptr, (bytes));	\
			assert(new_vec_ptr);								\
			(vec) = (void*)(&new_vec_ptr[2]);				\
			set_vector_capacity((vec), (count));		\
		}												\
	}while(0)

#define vector_pop_back(vec)							\
	do{													\
		set_vector_size((vec), get_vector_size(vec) - 1);	\
	}while(0)

#define vector_erase(vec, index)									\
	do{															\
		if(vec)													\
		{														\
			const size_t size = get_vector_size(vec);				\
			if ((index) < size)									\
			{													\
				set_vector_size((vec), size - 1);				\
				for (size_t i = (index); i != (size - 1); ++i)	\
					(vec)[i] = (vec)[i + 1];				\
			}													\
		}														\
	}while(0)

#define vector_free(vec)							\
	do{												\
		if(vec)										\
		{											\
			size_t* vec_ptr = &((size_t*)(vec))[-2];	\
			free(vec_ptr);								\
		}											\
	}while(0)

#define vector_begin(vec) (vec)

#define vector_end(vec)	\
	((vec) ? &((vec)[get_vector_size(vec)]) : NULL)

// 是否使用对数增长算法
#ifdef CVECTOR_LOGARITHMIC_GROWTH

#define vector_push_back(vec, value)								\
	do{																\
		size_t capacity = get_vector_capacity(vec);						\
		if (capacity <= get_vector_size(vec))							\
			vector_grow((vec), !capacity ? capacity + 1 : capacity * 2);	\
		vec[get_vector_size(vec)] = (value);							\
		set_vector_size((vec), get_vector_size(vec) + 1);				\
	}while(0)

#else

#define vector_push_back(vec, value)					\
	do{													\
		size_t capacity = get_vector_capacity(vec);			\
		if (capacity <= get_vector_size(vec))				\
			vector_grow((vec), capacity + 1);			\
		vec[get_vector_size(vec)] = (value);				\
		set_vector_size((vec), get_vector_size(vec) + 1);	\
	}while(0)

#endif /* CVECTOR_LOGARITHMIC_GROWTH */


#endif
