#pragma once

#include "mn/Exports.h"
#include "mn/Str.h"
#include "mn/Map.h"

namespace mn
{
	// string interning structure
	// string interning is an operation in which all of the unique strings is stored once
	// and every time a duplicate is encountered it returns a pointer to the same stored string
	// it's used mainly to avoid string compare functions since all you have to do now is compare
	// the string pointers if they are the same then they have the same content
	struct Str_Intern
	{
		Str tmp_str;
		Set<Str> strings;
	};

	// creates a new string interner
	inline static Str_Intern
	str_intern_new()
	{
		return Str_Intern {
			str_new(),
			set_new<Str>()
		};
	}

	// creates a new string interner with the given allocator
	inline static Str_Intern
	str_intern_with_allocator(Allocator allocator)
	{
		return Str_Intern {
			str_with_allocator(allocator),
			set_with_allocator<Str>(allocator)
		};
	}

	// frees the given string interner
	inline static void
	str_intern_free(Str_Intern& self)
	{
		str_free(self.tmp_str);
		destruct(self.strings);
	}

	// destruct overload for string intern free
	inline static void
	destruct(Str_Intern& self)
	{
		str_intern_free(self);
	}

	// interns the given a string and returns the string pointer to the interned string
	MN_EXPORT const char*
	str_intern(Str_Intern& self, const char* str);

	// interns the given a string and returns the string pointer to the interned string
	MN_EXPORT const char*
	str_intern(Str_Intern& self, const Str& str);

	// interns the given a string and returns the string pointer to the interned string
	MN_EXPORT const char*
	str_intern(Str_Intern& self, const char* begin, const char* end);
}