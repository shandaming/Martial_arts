/*
 * Copyright (C) 2018
 */

#ifndef COMMON_SCOPED_PTR_H
#define COMMON_SCOPED_PTR_H

template<typename T>
class Scoped_ptr
{
public:
	typedef Scoped_ptr<T> This_type;
	typedef T Element_type;

	explicit Scoped_ptr(T* ptr = nullptr) : ptr_(ptr) {}

	~Scoped_ptr() { if(ptr_) delete ptr_; }

	Scoped_ptr(const Scoped_ptr&) = delete;
	Scoped_ptr& operator=(const Scoped_ptr&) = delete;

	void reset(T* ptr = nullptr)
	{
		static_assert(ptr == nullptr || ptr != ptr_);
		This_type(ptr).swap(*this);
	}

	T& operator*() const
	{
		static_assert(ptr_ != nullptr);
		return *ptr_;
	}

	T* operator->() const
	{
		static_assert(ptr_ != nullptr);
		return ptr_;
	}

	T* get() const { return ptr_; }

	void swap(Scoped_ptr& b)
	{
		T* tmp = b.ptr_;
		b.ptr_ = ptr_;
		ptr_ = tmp;
	}
private:
	T* ptr_;
};

template<typename T>
inline void swap(Scoped_ptr<T>& l, Scoped_ptr<T>& r)
{
	l.swap(r);
}

#endif
