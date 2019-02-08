#ifndef SIMPLE_ARRAY_HPP
#define SIMPLE_ARRAY_HPP

#include<stdexcept>
#include<cstdlib>
#include<iterator>
#include<utility>
#include<cassert>

namespace my
{
	template<class T, class F = std::less<T>>
	struct SimpleArrayPredicateAdapter
	{
		SimpleArrayPredicateAdapter() {}
		SimpleArrayPredicateAdapter(F f) : func{ f } {}
		constexpr bool operator()(T* left, T* right) const
		{
			assert(left != nullptr && right != nullptr && "null pointer was given");
			return func(*left, *right);
		}
	private:
		F func;
	};

	template<class T>
	class SimpleArray
	{
	public:
		using value_type = T;
		using size_type = size_t;
		
		SimpleArray() {}
		explicit SimpleArray(const size_t sz) : sz_{sz}
		{
			elem_ = reinterpret_cast<T**>(malloc((sizeof(T*) + sizeof(T)) * sz_));
			T* ptr = reinterpret_cast<T*>(elem_ + sz_);
			for(size_t i = 0; i < sz_; ++i)
			{
				elem_[i] = ptr + i;
				::new(reinterpret_cast<void*>(elem_[i])) T();
			}
		}
		explicit SimpleArray(const size_t sz, const T& val) : sz_{sz}
		{
			elem_ = reinterpret_cast<T**>(malloc((sizeof(T*) + sizeof(T)) * sz_));
			T* ptr = reinterpret_cast<T*>(elem_ + sz_);
			for(size_t i = 0; i < sz_; ++i)
			{
				elem_[i] = ptr + i;
				::new(reinterpret_cast<void*>(elem_[i])) T(val);
			}
		}

		SimpleArray(const SimpleArray& other) : sz_{ other.sz_ }
		{
			elem_ = reinterpret_cast<T**>(malloc((sizeof(T*) + sizeof(T)) * sz_));
			T* ptr = reinterpret_cast<T*>(elem_ + sz_);
			for(size_t i = 0; i < sz_; ++i)
			{
				elem_[i] = ptr + i;
				::new(reinterpret_cast<void*>(elem_[i])) T(*(other.elem_[i]));
			}
		}
		SimpleArray& operator=(const SimpleArray& other)
		{
			if(this == &other) return;
			if(sz_ != other)
			{
				throw std::invalid_argument{"other SimpleArray must have the same size for assign"};
			}
			for(size_t i = 0; i < sz_; ++i)
			{
				*(elem_[i]) = *(other.elem_[i]);
			}
			return *this;
		}

		SimpleArray(SimpleArray&& other) : sz_{other.sz_}
		{
			std::swap(this->elem_, other.elem_);
			std::swap(this->sz_, other.sz_);
		}
		SimpleArray& operator=(SimpleArray&& other)
		{
			if(this == &other) return *this;
			std::swap(this->elem_, other.elem_);
			std::swap(this->sz_, other.sz_);
			return *this;
		}

		~SimpleArray() 
		{
			for(size_t i = 0; i < sz_; ++i) elem_[i]->~T();
			free(elem_); 
		}

		T& operator[](const size_t index)
		{
			range_check(index);
			return *elem_[index];
		}
		const T& operator[](const size_t index) const
		{
			range_check(index);
			return *elem_[index];
		}

		inline T** data() noexcept { return elem_; }
		inline const T* const* data() const noexcept { return elem_; }
		inline size_t size() const noexcept { return sz_; }

		void swap(const SimpleArray& other) noexcept
		{
			std::swap(this->elem_, other.elem_);
			std::swap(this->sz_, other.sz_);
		}

		inline void swap_elems(const size_t left, const size_t right) noexcept 
		{ 
			range_check(left);
			range_check(right);
			std::swap(elem_[left], elem_[right]); 
		}

		bool operator==(const SimpleArray& other) noexcept
		{
			if(sz_ != other.sz_) return false;
			for(size_t i = 0; i < sz_; ++i)
			{
				if(*(elem_[i]) != *(other.elem_[i])) return false;
			}
			return true;
		}
		bool operator!=(const SimpleArray& other) noexcept
		{
			if(sz_ != other.sz_) return true;
			for(size_t i = 0; i < sz_; ++i)
			{
				if(*(elem_[i]) != *(other.elem_[i])) return true;
			}
			return false;
		}

		bool operator<(const SimpleArray& other) noexcept
		{
			for(size_t i = 0; (i < sz_) && (i < other.sz_); ++i)
			{
				if(*(elem_[i]) < *(other.elem_[i])) return true;
				if(*(other.elem_[i]) < *(elem_[i])) return false;
			}
			return (i == sz_) && (i != other.sz_);
		}
		bool operator>(const SimpleArray& other) noexcept
		{
			for(size_t i = 0; (i < sz_) && (i < other.sz_); ++i)
			{
				if(*(elem_[i]) > *(other.elem_[i])) return true;
				if(*(other.elem_[i]) > *(elem_[i])) return false;
			}
			return (i == sz_) && (i != other.sz_);
		}

		bool operator<=(const SimpleArray& other) noexcept
		{
			for(size_t i = 0; (i < sz_) && (i < other.sz_); ++i)
			{
				if(*(elem_[i]) > *(other.elem_[i])) return false;
			}
			return true;
		}
		bool operator>=(const SimpleArray& other) noexcept
		{
			for(size_t i = 0; (i < sz_) && (i < other.sz_); ++i)
			{
				if(*(elem_[i]) < *(other.elem_[i])) return false;
			}
			return true;
		}

		inline T** ptr_begin() noexcept { return elem_; }
		inline T** ptr_end() noexcept { return elem_ + sz_; }

		inline const T* const* ptr_begin() const noexcept { return elem_; }
		inline const T* const* ptr_end() const noexcept { return elem_ + sz_; }

		class SimpleArrayIteraror;
		class ConstSimpleArrayIteraror;

		using iterator = SimpleArrayIteraror;
		using const_iterator = ConstSimpleArrayIteraror;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		inline iterator begin() noexcept { return iterator(elem_); }
		inline iterator end() noexcept { return iterator(elem_ + sz_); }

		inline const_iterator begin() const noexcept { return const_iterator(elem_); }
		inline const_iterator end() const noexcept { return const_iterator(elem_ + sz_); }

		inline const_iterator cbegin() const noexcept { return const_iterator(elem_); }
		inline const_iterator cend() const noexcept { return const_iterator(elem_ + sz_); }

		inline reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		inline reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

		inline const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		inline const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

		inline const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }
		inline const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

	private:
		inline void range_check(size_t index) const
		{
			if(index >= sz_)
				throw std::out_of_range{"index is out of range"};
		}
		constexpr inline size_t align(const size_t x, const size_t a) const noexcept { return ((x-1) | (a-1)) + 1; }

	private:
		T** elem_{nullptr};
		size_t sz_{};
	};

	template<class T>
	class SimpleArray<T>::SimpleArrayIteraror : public std::iterator<std::random_access_iterator_tag, T>
	{
		friend class SimpleArray<T>;
	private:
		SimpleArrayIteraror(T** p) : p{ p } {}
	public:
		using value_type = T;
		using difference_type = size_t;
		using pointer = T * ;
		using reference = T & ;

		SimpleArrayIteraror(const SimpleArrayIteraror& it) noexcept : p{ it.p } {}

		inline bool operator!=(SimpleArrayIteraror const& other) const noexcept { return p != other.p; }
		inline bool operator==(SimpleArrayIteraror const& other) const noexcept { return p == other.p; }

		inline bool operator>(SimpleArrayIteraror const& other) const noexcept { return p > other.p; }
		inline bool operator<(SimpleArrayIteraror const& other) const noexcept { return p < other.p; }

		inline bool operator>=(SimpleArrayIteraror const& other) const noexcept { return p >= other.p; }
		inline bool operator<=(SimpleArrayIteraror const& other) const noexcept { return p <= other.p; }

		inline T& operator*() const noexcept { return *(*p); }
		inline T* operator->()const noexcept { return *p; }

		inline SimpleArrayIteraror operator++() noexcept
		{ // preincrement
			++p;
			return *this;
		}
		inline SimpleArrayIteraror operator++(int) noexcept
		{ // postincrement
			auto tmp_it = *this;
			++*this;
			return tmp_it;
		}

		inline SimpleArrayIteraror operator--() noexcept
		{ // predecrement
			--p;
			return *this;
		}
		inline SimpleArrayIteraror operator--(int) noexcept
		{ // postdecrement
			auto tmp_it = *this;
			--*this;
			return tmp_it;
		}

		inline SimpleArrayIteraror operator+(difference_type val) const noexcept { return SimpleArrayIteraror(p + val); }
		inline SimpleArrayIteraror operator+=(difference_type val) noexcept { p += val; return *this; }

		inline SimpleArrayIteraror operator-(difference_type val) const noexcept { return SimpleArrayIteraror(p - val); }
		inline SimpleArrayIteraror operator-=(difference_type val) noexcept { p -= val; return *this; }

		inline difference_type operator-(const SimpleArrayIteraror& other) const noexcept { return this->p - other.p; }

	private:
		T** p{ nullptr };
	};

	template<class T>
	class SimpleArray<T>::ConstSimpleArrayIteraror : public std::iterator<std::random_access_iterator_tag, T>
	{
		friend class SimpleArray<T>;
		friend class SimpleArray<T>::SimpleArrayIteraror;
	private:
		ConstSimpleArrayIteraror(T** p) noexcept : p{ p } {}
	public:
		using value_type = T;
		using difference_type = int;
		using pointer = T * ;
		using reference = T & ;

		ConstSimpleArrayIteraror(const ConstSimpleArrayIteraror& it) noexcept : p{ it.p } {}
		ConstSimpleArrayIteraror(const SimpleArray<T>::SimpleArrayIteraror& it) noexcept : p{ it.p } {}

		inline bool operator!=(ConstSimpleArrayIteraror const& other) const noexcept { return p != other.p; }
		inline bool operator==(ConstSimpleArrayIteraror const& other) const noexcept { return p == other.p; }

		inline bool operator>(ConstSimpleArrayIteraror const& other) const noexcept { return p > other.p; }
		inline bool operator<(ConstSimpleArrayIteraror const& other) const noexcept { return p < other.p; }

		inline bool operator>=(ConstSimpleArrayIteraror const& other) const noexcept { return p >= other.p; }
		inline bool operator<=(ConstSimpleArrayIteraror const& other) const noexcept { return p <= other.p; }

		inline const T& operator*() const noexcept { return *(*p); }
		inline const T* operator->() const noexcept { return *p; }

		inline ConstSimpleArrayIteraror operator++() noexcept
		{ // preincrement
			++p;
			return *this;
		}
		inline ConstSimpleArrayIteraror operator++(int) noexcept
		{ // postincrement
			auto tmp_it = *this;
			++*this;
			return tmp_it;
		}

		inline ConstSimpleArrayIteraror operator--() noexcept
		{ // predecrement
			--p;
			return *this;
		}
		inline ConstSimpleArrayIteraror operator--(int) noexcept
		{ // postdecrement
			auto tmp_it = *this;
			--*this;
			return tmp_it;
		}

		inline ConstSimpleArrayIteraror operator+(difference_type val) const noexcept { return ConstSimpleArrayIteraror(p + val); }
		inline ConstSimpleArrayIteraror operator+=(difference_type val) noexcept { p += val; return *this; }

		inline ConstSimpleArrayIteraror operator-(difference_type val) const noexcept { return ConstSimpleArrayIteraror(p - val); }
		inline ConstSimpleArrayIteraror operator-=(difference_type val) noexcept { p -= val; return *this; }

		inline difference_type operator-(const ConstSimpleArrayIteraror& other) const noexcept { return this->p - other.p; }

	private:
		T** p{ nullptr };
	};

}

#endif // SIMPLE_ARRAY_HPP
