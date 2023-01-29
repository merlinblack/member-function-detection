Conceptual fun in C++20
=======================

This week I was reviewing one of my libraries - [manualbind](https://github.com/merlinblack/manualbind) - that I had made to interface C++ and Lua code, helping make C++ classes available to Lua code.

One of the techniques I had used is based on on a 2015 paper for implementing template detection, which allows you to specialise a template depending on the existence of a member function for example. 

http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf

I was using this to either call a object creation member function if it existed in the class binding, or return a Lua error saying you can't make one if the function was not defined. No virtual methods or inheritance required, and the choice is made at compilation.

But at the time of writing this - it's early 2023. Had that technique made it into the C++ standard?

Almost there
------------

Well it did! But only as an experimental feature. Lets have a look...

```language=C++
#include <experimental/type_traits>

using std::experimental::is_detected;

template<typename T>
using create_t = decltype(std::declval<T>().create());

template<typename T>
using has_create = is_detected<create_t, T>;
```

This allows us to specialise depending on the "type" of has_create.

```language=C++
// Generic, used when no create member function exists.
template<typename T>
void call_create(T& t, std::true_type)
{
	cout << "Calling T.create()\n";
	t.create();
}

// Specialisation for having a create member function.
template<typename T>
void call_create(T& t, std::false_type)
{
	cout << "No T.create() to call\n";
}

template<typename T>
void call_create(T& t)
{
	has_create<T> createTrait;

	call_create(t, createTrait);
}
```

and finally try it out...
```
int main()
{
	Can can;
	Cannot cannot;

	call_create(can);
	call_create(cannot);
	return 0;
}
```

This prints:
```
Calling T.create()
Struct member 'create()'
No T.create() to call
```

Lovely. But why is it still only "experimental"? After some research of the googling kind, I found that your mileage may vary depending on what compiler you are using. (gcc 12 for me right now) I didn't feel the need to update my library code to use this.

A Tour of C++
-------------
I've been reading the third edition of this great book by Bjarne Stroustrup himself, which has been updated for C++20. After some reading up on 'concepts' and some more googling including these two gems:

https://www.codeproject.com/Articles/5340890/An-Introduction-to-Cplusplus-Concepts-for-Template

https://www.cppstories.com/2019/07/detect-overload-from-chars/

I found the C++20 solution, while similar, is much more compact.

Here is the same program as above - but using a concept.
```language=C++
#include <iostream>
#include <concepts>

using std::cout;

template<typename T>
concept has_create = requires(T t) { {t.create()} -> std::same_as<void>; };

struct Can {
	void create() { cout << "Struct member 'create()'\n"; }
};

struct Cannot {
};

static_assert(has_create<Can>, "Can should be able to create!");
static_assert(!has_create<Cannot>, "Cannot should not be able to create!");

template<typename T>
void call_create(T& t)
{
	cout << "No create member to call.\n";
}

template<has_create T>
void call_create(T& t)
{
	cout << "Calling create member:\n";
	t.create();
}

int main()
{
	Can can;
	Cannot cannot;

	call_create(can);
	call_create(cannot);
	return 0;
}
```

Note that now we only need two 'call_create' functions, a generic one, and a specialised one that matches the concept. Before we needed two specialisations for the std::true, and std::false types and another one to call the correct specialisation.

The definition of the concept has_create means that it is "constexpr(true)" for any type T that has a create member returning void.

The call_create specialisation uses a new C++20 shortcut in the template definition that requires T to fulfill the requirements of the has_create concept.

The concepts can be a bit tricky to write. Here is an example for a member function that has parameters and returns an int.

```language=C++
template<typename T>
concept has_create = requires(T t, lua_State* L) { {t.create(L)} -> std::same_as<int>; };
```

It took me a few goes to get that right, the syntax is not really something you can guess. Anyhow, I think I have the hang of it now.

Full code
---------
Find the full code and a more full example at:

https://github.com/merlinblack/member-function-detection
