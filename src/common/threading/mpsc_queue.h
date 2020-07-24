/*
 * Copyright (C) 2020
 */

#ifndef THREAD_MPSC_QUEUE_H
#define THREAD_MPSC_QUEUE_H

#include <atomic>
//#include <utility>

template <typename T>
class mpsc_queue
{
public:
	mpsc_queue() : head_(new node()), tail_(head_.load(std::memory_order_relaxed))
	{
		node* front = head_.load(std::memory_order_relaxed);
		front->next.store(nullptr, std::memory_order_relaxed);
	}

	~mpsc_queue()
	{
		T* output;
		while(this->dequeue(output))
			;

		node* front = head_.load(std::memory_order_relaxed);
		delete front;
	}

	void enqueue(T* input)
	{
		node* n = new node(input);
		node* prev_head = head_.exchange(n, std::memory_order_acq_rel);
		prev_head->next.store(n, std::memory_order_release);
	}

	bool dequeue(T*& result)
	{
		node* tail = tail_.load(std::memory_order_relaxed);
		node* next = tail->next.load(std::memory_order_acquire);
		if(!next)
			return false;

		result = next->data;
		tail_.store(next, std::memory_order_release);
		delete tail;
		return true;
	}
private:
	mpsc_queue(const mpsc_queue&) = delete;
	mpsc_queue& operator=(const mpsc_queue&) = delete;

	struct node
	{
		node() = default;
		explicit node(T* data_) : data(data_)
		{
			next.store(nullptr, std::memory_order_relaxed);
		}

		T* data;
		std::atomic<node*> next;
	};

	std::atomic<node*> head_;
	std::atomic<node*> tail_;
};

#endif
