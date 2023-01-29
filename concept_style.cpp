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
