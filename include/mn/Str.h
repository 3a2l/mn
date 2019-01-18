#pragma once

#include "mn/Exports.h"
#include "mn/Buf.h"
#include "mn/Map.h"

#include <string.h>
#include <stdio.h>
#include <utility>

namespace mn
{
	/**
	 * A String is just a `Buf<char>` so it's suitable to use the buf functions with the string
	 * but use with caution since the null terminator should be maintained all the time
	 */
	using Str = Buf<char>;

	/**
	 * @brief      Returns a new string
	 */
	API_MN Str
	str_new();

	/**
	 * @brief      Returns a new string
	 *
	 * @param[in]  allocator  The allocator to be used by the string
	 */
	API_MN Str
	str_with_allocator(Allocator allocator);

	/**
	 * @brief      Returns a new string which has the same content as the given
	 * C string (performs a copy)
	 *
	 * @param[in]  str        The C string
	 * @param[in]  allocator  The allocator
	 */
	API_MN Str
	str_from_c(const char* str, Allocator allocator = allocator_top());

	/**
	 * @brief      Returns a new string which has the same content of the given
	 * C sub string (performs a copy)
	 *
	 * @param[in]  begin      The begin
	 * @param[in]  end        The end
	 * @param[in]  allocator  The allocator
	 */
	API_MN Str
	str_from_substr(const char* begin, const char* end, Allocator allocator = allocator_top());

	/**
	 * @brief      Just wraps the given C string literal (doesn't copy)
	 *
	 * @param[in]  lit   The C string literal
	 */
	API_MN Str
	str_lit(const char* lit);

	/**
	 * @brief      Frees the string
	 */
	API_MN void
	str_free(Str& self);

	/**
	 * @brief      Destruct function overload for the string
	 *
	 * @param      self  The string
	 */
	inline static void
	destruct(Str& self)
	{
		str_free(self);
	}

	/**
	 * @brief      Returns the rune(utf-8 chars) count in the given string
	 *
	 * @param[in]  str   The string
	 */
	API_MN size_t
	rune_count(const char* str);

	/**
	 * @brief      Returns the size(in bytes) of the given runes
	 *
	 * @param[in]  r     The rune
	 */
	inline static size_t
	rune_size(int32_t r)
	{
		char* b = (char*)&r;
		return ((b[0] != 0) +
				(b[1] != 0) +
				(b[2] != 0) +
				(b[3] != 0));
	}

	/**
	 * @brief      Given a string iterator/pointer it will move it to point to the next rune
	 *
	 * @param[in]  str   The string
	 */
	inline static const char*
	rune_next(const char* str)
	{
		++str;
		while(*str && ((*str & 0xC0) == 0x80))
			++str;
		return str;
	}

	/**
	 * @brief      Extract a rune from the given string
	 *
	 * @param[in]  c     The string
	 */
	inline static int32_t
	rune_read(const char* c)
	{
		if(c == nullptr)
			return 0;

		if(*c == 0)
			return 0;

		int32_t rune = 0;
		uint8_t* result = (uint8_t*)&rune;
		const uint8_t* it = (const uint8_t*)c;
		*result++ = *it++;
		while (*it && ((*it & 0xC0) == 0x80))
			*result++ = *it++;
		return rune;
	}

	/**
	 * @brief      Returns the rune count of the given string
	 *
	 * @param[in]  self  The string
	 */
	inline static size_t
	str_rune_count(const Str& self)
	{
		if(self.count)
			return rune_count(self.ptr);
		return 0;
	}

	/**
	 * @brief      Pushes the second string into the first one
	 *
	 * @param      self  The first string
	 * @param[in]  str   The second string literal
	 */
	API_MN void
	str_push(Str& self, const char* str);

	/**
	 * @brief      Pushes the second string into the first one
	 *
	 * @param      self  The first string
	 * @param[in]  str   The second string
	 */
	inline static void
	str_push(Str& self, const Str& str)
	{
		str_push(self, str.ptr);
	}

	/**
	 * @brief      Pushes a block of memory into the string
	 *
	 * @param      self   The string
	 * @param[in]  block  The block
	 */
	API_MN void
	str_block_push(Str& self, Block block);

	/**
	 * @brief      uses printf family of functions to write a formatted string into the string
	 *
	 * @param      self       The string
	 * @param[in]  fmt        The format
	 * @param[in]  args       The arguments of the printf
	 */
	template<typename ... TArgs>
	inline static void
	str_pushf(Str& self, const char* fmt, TArgs&& ... args)
	{
		size_t size = self.cap - self.count;
		size_t len = ::snprintf(self.ptr + self.count, size, fmt, std::forward<TArgs>(args)...) + 1;
		if(len > size)
		{
			buf_reserve(self, len);
			size = self.cap - self.count;
			len = ::snprintf(self.ptr + self.count, size, fmt, std::forward<TArgs>(args)...) + 1;
			assert(len <= size);
		}
		self.count += len - 1;
	}

	/**
	 * @brief      Ensures that the string is null terminated
	 *
	 * @param      self  The string
	 */
	API_MN void
	str_null_terminate(Str& self);

	/**
	 * @brief      Clears the string
	 *
	 * @param      self  The string
	 */
	API_MN void
	str_clear(Str& self);

	/**
	 * @brief      Clones the given string
	 *
	 * @param[in]  other      The string
	 * @param[in]  allocator  The allocator to be used in the returned string
	 *
	 * @return     The newly cloned string
	 */
	API_MN Str
	str_clone(const Str& other, Allocator allocator = allocator_top());

	/**
	 * @brief      Clone function overload for the string type
	 *
	 * @param[in]  other  The string to be cloned
	 *
	 * @return     Copy of this object.
	 */
	inline static Str
	clone(const Str& other)
	{
		return str_clone(other);
	}

	template<>
	struct Hash<Str>
	{
		inline size_t
		operator()(const Str& str) const
		{
			return str.count ? murmur_hash(str.ptr, str.count) : 0;
		}
	};

	inline static int
	str_cmp(const char* a, const char* b)
	{
		if (a != nullptr && b != nullptr)
		{
			return ::strcmp(a, b);
		}
		else if (a == nullptr && b == nullptr)
		{
			return 0;
		}
		else if (a != nullptr && b == nullptr)
		{
			if (::strlen(a) == 0)
				return 0;
			else
				return 1;
		}
		else if (a == nullptr && b != nullptr)
		{
			if (::strlen(b) == 0)
				return 0;
			else
				return -1;
		}
		else
		{
			assert(false && "UNREACHABLE");
			return 0;
		}
	}

	inline static bool
	operator==(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) == 0;
	}

	inline static bool
	operator!=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) != 0;
	}

	inline static bool
	operator<(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) < 0;
	}

	inline static bool
	operator<=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) <= 0;
	}

	inline static bool
	operator>(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) > 0;
	}

	inline static bool
	operator>=(const Str& a, const Str& b)
	{
		return str_cmp(a.ptr, b.ptr) >= 0;
	}


	inline static bool
	operator==(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) == 0;
	}

	inline static bool
	operator!=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) != 0;
	}

	inline static bool
	operator<(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) < 0;
	}

	inline static bool
	operator<=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) <= 0;
	}

	inline static bool
	operator>(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) > 0;
	}

	inline static bool
	operator>=(const Str& a, const char* b)
	{
		return str_cmp(a.ptr, b) >= 0;
	}


	inline static bool
	operator==(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) == 0;
	}

	inline static bool
	operator!=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) != 0;
	}

	inline static bool
	operator<(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) < 0;
	}

	inline static bool
	operator<=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) <= 0;
	}

	inline static bool
	operator>(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) > 0;
	}

	inline static bool
	operator>=(const char* a, const Str& b)
	{
		return str_cmp(a, b.ptr) >= 0;
	}
}