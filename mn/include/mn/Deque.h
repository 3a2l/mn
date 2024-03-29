#pragma once

#include "mn/Memory.h"
#include "mn/Assert.h"

#include <string.h>

namespace mn
{
	struct Deque_Index
	{
		// index of the bucket in array
		size_t bucket_index;
		// index of the element inside a bucket
		size_t element_index;
	};

	// a double ended queue which allows you to push to either sides of the array
	template<typename T>
	struct Deque
	{
		Allocator allocator;
		T** buckets;
		size_t count;
		size_t cap;
		size_t bucket_count;
		size_t bucket_cap;
		Deque_Index front;
		Deque_Index back;
		size_t bucket_size;

		T&
		operator[](size_t ix)
		{
			mn_assert(ix < count);
			size_t bucket_index = ix / bucket_size;
			size_t element_index = ix % bucket_size;

			bucket_index += front.bucket_index;
			element_index += front.element_index;

			bucket_index += element_index / bucket_size;
			element_index = element_index % bucket_size;
			return buckets[bucket_index][element_index];
		}

		const T&
		operator[](size_t ix) const
		{
			mn_assert(ix < count);
			size_t bucket_index = ix / bucket_size;
			size_t element_index = ix % bucket_size;

			bucket_index += front.bucket_index;
			element_index += front.element_index;

			bucket_index += element_index / bucket_size;
			element_index = element_index % bucket_size;
			return buckets[bucket_index][element_index];
		}
	};

	// creates a new instance of a deque
	template<typename T>
	inline static Deque<T>
	deque_new()
	{
		Deque<T> self{};
		self.allocator = allocator_top();
		self.buckets = nullptr;
		self.count = 0;
		self.cap = 0;
		self.bucket_count = 0;
		self.bucket_cap = 0;
		self.front = { 0, 0 };
		self.back = { 0, 0 };
		self.bucket_size = sizeof(T) >= 4096 ? 1 : (4096 / sizeof(T));
		return self;
	}

	// creates a new deque instance with the given allocator
	template<typename T>
	inline static Deque<T>
	deque_with_allocator(Allocator allocator)
	{
		Deque<T> self{};
		self.allocator = allocator;
		self.buckets = nullptr;
		self.count = 0;
		self.cap = 0;
		self.bucket_count = 0;
		self.bucket_cap = 0;
		self.front = { 0, 0 };
		self.back = { 0, 0 };
		self.bucket_size = sizeof(T) >= 4096 ? 1 : (4096 / sizeof(T));
		return self;
	}

	// frees the given deque instance
	template<typename T>
	inline static void
	deque_free(Deque<T>& self)
	{
		for (size_t i = 0; i < self.bucket_count; ++i)
			free_from(self.allocator, Block{ self.buckets[i], sizeof(T) * self.bucket_size });
		free_from(self.allocator, Block{ self.buckets, self.bucket_cap * sizeof(T*) });
	}

	// a custom overload for deque which loops over all the elements and calls destruct, this is useful for destructing
	// a big hierarchy without memory leaks, for example when you free mn::Deque<mn::Deque<int>> if you use mn::deque_free
	// you'll only free the top level deque and won't free the small mn::Deque<int> inside, it's more appropriate to use
	// destruct instead of mn::deque_free for this case which will destruct each mn::Deque<int> inside the bigger deque
	template<typename T>
	inline static void
	destruct(Deque<T>& self)
	{
		for (size_t i = 0; i < self.count; ++i)
			destruct(self[i]);
		deque_free(self);
	}

	template<typename T>
	inline static bool
	deque_index_can_inc(const Deque<T>& self, Deque_Index index)
	{
		if (index.element_index + 1 < self.bucket_size)
			return true;

		if (index.bucket_index + 1 < self.bucket_cap)
			return true;

		return false;
	}

	template<typename T>
	inline static Deque_Index
	deque_index_inc(const Deque<T>& self, Deque_Index index)
	{
		if (index.element_index + 1 >= self.bucket_size)
		{
			index.element_index = 0;
			++index.bucket_index;
			return index;
		}
		else
		{
			++index.element_index;
			return index;
		}
	}

	template<typename T>
	inline static bool
	deque_index_can_dec(const Deque<T>& self, Deque_Index index)
	{
		if (index.element_index > 0)
			return true;

		if (index.bucket_index > 0)
			return true;

		return false;
	}

	template<typename T>
	inline static Deque_Index
	deque_index_dec(Deque<T>& self, Deque_Index index)
	{
		if (index.element_index == 0)
		{
			index.element_index = self.bucket_size - 1;
			--index.bucket_index;
			return index;
		}
		else
		{
			--index.element_index;
			return index;
		}
	}

	template<typename T>
	inline static void
	deque_grow_back(Deque<T>& self)
	{
		if(self.cap > self.count && self.back.bucket_index < self.bucket_count && deque_index_can_inc(self, self.back))
			return;

		T* bucket = (T*)alloc_from(self.allocator, sizeof(T) * self.bucket_size, alignof(T)).ptr;

		// then we need to resize this array
		if (self.bucket_count >= self.bucket_cap)
		{
			size_t cap = self.bucket_cap == 0 ? 8 : self.bucket_cap * 2;
			T** new_bucket_array = (T**)alloc_from(self.allocator, cap * sizeof(T*), alignof(T*)).ptr;
			if (self.buckets != nullptr)
			{
				::memcpy(new_bucket_array, self.buckets, self.bucket_count * sizeof(T*));
				free_from(self.allocator, Block{ self.buckets, self.bucket_cap * sizeof(T*) });
			}
			self.buckets = new_bucket_array;
			self.bucket_cap = cap;
		}

		self.buckets[self.bucket_count] = bucket;
		self.cap += self.bucket_size;
		++self.bucket_count;
	}

	// pushes the given value to the back of the deque
	template<typename T>
	inline static void
	deque_push_back(Deque<T>& self, const T& v)
	{
		deque_grow_back(self);
		self.buckets[self.back.bucket_index][self.back.element_index] = v;
		self.back = deque_index_inc(self, self.back);
		++self.count;
	}

	// allocates space for a single element at the back of the deque and returns a pointer to it
	template<typename T>
	inline static T*
	deque_alloc_back(Deque<T>& self)
	{
		deque_grow_back(self);
		T* p = &self.buckets[self.back.bucket_index][self.back.element_index];
		self.back = deque_index_inc(self, self.back);
		++self.count;
		return p;
	}

	template<typename T>
	inline static void
	deque_grow_front(Deque<T>& self)
	{
		if (self.cap > self.count && deque_index_can_dec(self, self.front))
			return;

		T* bucket = (T*)alloc_from(self.allocator, sizeof(T) * self.bucket_size, alignof(T)).ptr;

		if (self.bucket_count < self.bucket_cap)
		{
			::memmove(self.buckets + 1, self.buckets, self.bucket_count * sizeof(T*));
		}
		else
		{
			size_t cap = self.bucket_cap == 0 ? 8 : self.bucket_cap * 2;
			T** new_bucket_array = (T**)alloc_from(self.allocator, cap * sizeof(T*), alignof(T*)).ptr;
			if (self.buckets)
			{
				::memcpy(new_bucket_array + 1, self.buckets, self.bucket_count * sizeof(T*));
				free_from(self.allocator, Block{ self.buckets, self.bucket_cap * sizeof(T*) });
			}
			self.buckets = new_bucket_array;
			self.bucket_cap = cap;
		}

		self.buckets[0] = bucket;
		self.cap += self.bucket_size;
		++self.bucket_count;

		++self.front.bucket_index;
		++self.back.bucket_index;
		deque_grow_back(self);
	}

	// pushes the given value to the front of the deque
	template<typename T>
	inline static void
	deque_push_front(Deque<T>& self, const T& v)
	{
		deque_grow_front(self);
		self.front = deque_index_dec(self, self.front);
		self.buckets[self.front.bucket_index][self.front.element_index] = v;
		++self.count;
	}

	// allocates space for a single element at the front of the deque and returns a pointer to it
	template<typename T>
	inline static T*
	deque_alloc_front(Deque<T>& self)
	{
		deque_grow_front(self);
		self.front = deque_index_dec(self, self.front);
		T* p = &self.buckets[self.front.bucket_index][self.front.element_index];
		++self.count;
		return p;
	}

	// removes an element off the back of the given deque
	template<typename T>
	inline static void
	deque_pop_back(Deque<T>& self)
	{
		if (self.count == 0)
			return;
		self.back = deque_index_dec(self, self.back);
		--self.count;
	}

	// removes an element off the front of the given deque
	template<typename T>
	inline static void
	deque_pop_front(Deque<T>& self)
	{
		if (self.count == 0)
			return;
		self.front = deque_index_inc(self, self.front);
		--self.count;
	}

	// returns a reference to the front of the given deque
	template<typename T>
	inline static T&
	deque_front(Deque<T>& self)
	{
		return self[0];
	}

	// returns a reference to the front of the given deque
	template<typename T>
	inline static const T&
	deque_front(const Deque<T>& self)
	{
		return self[0];
	}

	// returns a reference to the back of the given deque
	template<typename T>
	inline static T&
	deque_back(Deque<T>& self)
	{
		return self[self.count - 1];
	}

	// returns a reference to the back of the given deque
	template<typename T>
	inline static const T&
	deque_back(const Deque<T>& self)
	{
		return self[self.count - 1];
	}

	// a custom clone function for the deque, which iterators over the elements and calls clone on each one of them
	// thus making a deep copy of the deque
	template<typename T>
	inline static Deque<T>
	deque_clone(const Deque<T>& other, Allocator allocator = allocator_top())
	{
		Deque<T> res = deque_with_allocator<T>(allocator);
		for (size_t i = 0; i < other.count; ++i)
			deque_push_back(res, clone(other[i]));
		return res;
	}

	// an overload of the general clone function which uses the custom clone function of the deque which iterators over
	// the elements and calls clone on each one of them thus making a deep copy of the deque
	template<typename T>
	inline static Deque<T>
	clone(const Deque<T>& other)
	{
		return deque_clone(other);
	}
}