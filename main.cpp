#include<iostream>
#include<algorithm>
#include<vector>
#include<numeric>
#include"SimpleArray.hpp"
#include"SimpleTimer.hpp"

struct Value // fatty object
{
	Value() noexcept {}
	Value(int v) noexcept : value{v} {}
	Value(const Value& other) noexcept
		: value{other.value}, 
		a{other.a}, 
		b{other.b}, 
		c{other.c}, 
		d{other.d},
		e{other.e},
		f{other.f},
		g{other.g}
	{}
	Value& operator=(const Value& other) noexcept // assign is very expensive
	{
		if(this == &other) return *this;

		value = other.value;
		a = other.a;
		b = other.b;
		c = other.c;
		d = other.d;
		e = other.e;
		f = other.f;
		g = other.g;

		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Value& val)
	{
		os << val.value;
		return os;
	}

	inline bool operator>(const Value& other) const noexcept { return this->value > other.value; }
	inline bool operator<(const Value& other) const noexcept { return this->value < other.value; }

	inline bool operator>=(const Value& other) const noexcept { return this->value >= other.value; }
	inline bool operator<=(const Value& other) const noexcept { return this->value <= other.value; }

	inline bool operator==(const Value& other) const noexcept { return this->value == other.value; }
	inline bool operator!=(const Value& other) const noexcept { return this->value != other.value; }

private:
	// value
	long long value{};
	// pile of junk
	long long a{};
	long long b{};
	long long c{};
	long long d{};
	long long e{};
	long long f{};
	long long g{};
};

int main(int argc, char* argv[]) 
{
	my::SimpleTimer<std::chrono::microseconds> timer;

	constexpr int count_elems = 10000;

	// my::SimpleArray test
	timer.start();
	my::SimpleArray<Value> arr(count_elems);
	std::iota(std::begin(arr), std::end(arr), 1);

	std::stable_sort(arr.ptr_begin(), arr.ptr_end(), my::SimpleArrayPredicateAdapter<Value, std::greater<Value>>());

	my::SimpleArray<Value> arr2(arr);
	std::iota(std::begin(arr2), std::end(arr2), 1);
	std::sort(arr2.ptr_begin(), arr2.ptr_end(), my::SimpleArrayPredicateAdapter<Value, std::greater<Value>>());

	arr = std::move(arr2);

	timer.stop();
	std::cout << "Time of my::SimpleArray<Value>: " << timer.elapsed_time().count() << std::endl;


	// std::vector test
	timer.start();
	std::vector<Value> vec(count_elems);
	std::iota(std::begin(vec), std::end(vec), 1);

	std::stable_sort(std::begin(vec), std::end(vec), std::greater<Value>());

	std::vector<Value> vec2(vec);
	std::iota(std::begin(vec2), std::end(vec2), 1);
	std::sort(std::begin(vec), std::end(vec), std::greater<Value>());

	vec = std::move(vec2);

	timer.stop();
	std::cout << "Time of std::vector<Value>: " << timer.elapsed_time().count() << std::endl;

	return 0;
}
