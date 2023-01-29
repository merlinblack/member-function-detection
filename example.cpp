#include <iostream>
#include <concepts>

using std::cout;

struct lua_State
{};

template<typename T>
concept has_create = requires(T t, lua_State* L) { {t.create(L)} -> std::same_as<int>; };

struct Can {
	int create(lua_State* L)
 	{ 
		cout << "Struct member 'create()'\n";
		return 0;
	}
};

struct Cannot {
};

static_assert(has_create<Can>, "Can should be able to create!");
static_assert(!has_create<Cannot>, "Cannot should not be able to create!");

// Generic, used when no create member function exists.
template<typename T>
void call_create(T& t, lua_State* L)
{
	cout << "No create member to call.\n";
}

// Specialisation for having a create member function.
template<has_create T>
void call_create(T& t, lua_State* L)
{
	cout << "Calling create member:\n";
	t.create(L);
}

int main()
{
	Can can;
	Cannot cannot;

	lua_State* L = nullptr;

	call_create(can, L);
	call_create(cannot, L);
	return 0;
}
