#ifndef VECTOR_H
#define VECTOR_H
#include<iostream>
#include<stdexcept>
#include<memory>
#include<initializer_list>
#include<algorithm>

template <typename T>
class Vector {

public:
	class ConstIterator;
	class Iterator;
	using value_type = T;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using iterator = Vector::Iterator;
	using const_iterator = Vector::ConstIterator;

private:
	//static constexpr value_type allocation_factor = 1.61;
	static constexpr std::size_t start_size = 5;

	std::size_t sz;
	std::size_t max_sz;
	pointer values;
	static constexpr size_t min_sz{ 5 };

public:
	//Methods
	Vector() : sz{ 0 }, max_sz{ min_sz }, values{ new value_type[max_sz] }{
	}

	Vector(std::size_t n) : Vector() {
		reserve(n);
	}

	Vector(std::initializer_list<value_type> inhalt) : Vector(inhalt.size()) {
		for (const auto& v : inhalt)
			values[sz++] = v;
	}

	Vector(const Vector& r) : Vector(r.sz)
	{
		this->sz = r.size();
		this->max_sz = r.capacity();
		this->values = new value_type[r.capacity()];
		for (std::size_t i{ 0 }; i < r.size(); i++)
			this->values[i] = r[i];
	}

	~Vector() {
		delete[] values;
	}

	void clear() { sz = 0; }

	size_t size() const { return sz; }

	bool empty() const {
		return sz == 0;
	}

	void shrink_to_fit() {
		if (sz == 0) {
			delete[] values;
			values = nullptr;
			max_sz = 0;
		}
		else if (max_sz == sz)
			return;
		else {
			pointer next{ new value_type[sz] };

			for (std::size_t i{ 0 }; i < sz; i++)
				next[i] = values[i];

			delete[] values;
			values = next;
			max_sz = sz;
		}
	}

	Vector& operator=(Vector v) {
		std::swap(sz, v.sz);
		std::swap(max_sz, v.max_sz);
		std::swap(values, v.values);
		return *this;
	}
	void reserve(std::size_t n) {
		if (n <= 0 || max_sz >= n) return;

		pointer next{ new value_type[n] };

		for (std::size_t i{ 0 }; i < sz; ++i) {
			next[i] = values[i];
		}

		delete[] values;
		values = next;
		max_sz = n;
	}

	void push_back(const value_type& elem) {
		if (sz == max_sz)
			reserve(sz * 2 + 1);
		values[sz++] = elem;
	}

	void pop_back() {
		if (sz == 0)
			throw std::runtime_error{ "No elements in vec!" };
		--sz;
	}

	value_type& operator[](size_t index) {
		if (index >= sz)
			throw std::runtime_error("Requested position is out of range!");
		return values[index];
	}

	const value_type& operator[](size_t index) const {
		if (index >= sz)
			throw std::runtime_error("Requested position is out of range!");
		return values[index];
	}
	size_t capacity() const { return max_sz; }

	std::ostream& print(std::ostream& o) const {
		o << "[";
		bool first{ true };
		for (std::size_t i{ 0 }; i < sz; ++i) {
			if (first) {
				o << values[i];
				first = false;
			}
			else
				o << ", " << values[i];
		}
		o << "]";
		return o;
	}

	iterator begin() { return values; }
	iterator end() { return values + sz; }
	const_iterator begin() const { return values; }
	const_iterator end() const { return values + sz; }

	class Iterator {
	public:
		using value_type = Vector::value_type;
		using reference = Vector::reference;
		using pointer = Vector::pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;
	private:
		pointer ptr;
	public:
		//Methods
		operator const_iterator() const {
			return const_iterator{ ptr };
		}
		Iterator(pointer ptr = nullptr) : ptr{ ptr } {
		}
		reference operator*() {
			return *ptr;
		}
		pointer operator->() {
			return ptr;
		}
		bool operator==(const const_iterator& iter) const {
			return iter == *this;
		}
		bool operator!=(const const_iterator& iter) const {
			return iter != *this;
		}
		iterator& operator++() {
			++ptr;
			return *this;
		}
		iterator operator++(int) {
			return ptr++;
		}
		Vector::difference_type operator-(const const_iterator& it) const {
			return ptr - it.ptr;
		}
	};

	class ConstIterator {
	public:
		using value_type = Vector::value_type;
		using reference = Vector::const_reference;
		using pointer = Vector::const_pointer;
		using difference_type = Vector::difference_type;
		using iterator_category = std::forward_iterator_tag;
	private:
		pointer ptr;
	public:
		//Methods
		ConstIterator(pointer ptr = nullptr) : ptr{ ptr } {
		}

		reference operator*() {
			return *ptr;
		}
		pointer operator->() const {
			return ptr;
		}
		bool operator==(const const_iterator& other) const {
			return ptr == other.ptr;
		}
		bool operator!=(const const_iterator& iter) const {
			return ptr != iter.ptr;
		}
		const_iterator& operator++() {
			ptr += 1;
			return *this;
		}
		const_iterator operator++(int) {
			ConstIterator help(ptr);
			ptr += 1;
			return help;
		}

		Vector::difference_type operator-(const const_iterator& it) const {
			return ptr - it.ptr;
		}
		/*friend Vector::difference_type operator-(const Vector::ConstIterator& lop,
			const Vector::ConstIterator& rop) {
			return lop.ptr - rop.ptr;
		}*/
	};

	iterator insert(const_iterator pos, const_reference val)
	{
		auto diff = pos - begin();
		if (diff < 0 || static_cast<size_type>(diff) > sz)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{ static_cast<size_type>(diff) };
		if (sz >= max_sz)
			reserve(max_sz * 2);
		for (size_type i{ sz }; i-- > current;)
			values[i + 1] = values[i];
		values[current] = val;
		++sz;
		return iterator{ values + current };
	}

	iterator erase(const_iterator pos)
	{
		auto diff = pos - begin();
		if (diff < 0 || static_cast<size_type>(diff) >= sz)
			throw std::runtime_error("Iterator out of bounds");
		size_type current{ static_cast<size_type>(diff) };
		for (auto i{ current }; i < sz - 1; ++i)
			values[i] = values[i + 1];
		--sz;
		return iterator{ values + current };
	}

};

template <typename T>
inline std::ostream& operator<<(std::ostream& o, const Vector<T>& v) {
	return v.print(o);
}

#endif

