#include <experimental/type_traits>
#include <iostream>

using std::experimental::is_detected;
using std::cout;

template<typename T>
using create_t = decltype(std::declval<T>().create());

template<typename T>
using has_create = is_detected<create_t, T>;

struct Can {
	void create() { cout << "Struct member 'create()'\n"; }
};

struct Cannot {
};

static_assert(has_create<Can>::value, "Can should be able to create!");
static_assert(!has_create<Cannot>::value, "Cannot should not be able to create!");

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

int main()
{
	Can can;
	Cannot cannot;

	call_create(can);
	call_create(cannot);
	return 0;
}
