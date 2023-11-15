#pragma once

#include <functional>
#include <stdexcept>
#include <string>

namespace rana {

class value
{
public:
	// Member types
	typedef std::size_t size_type;
	typedef std::ptrdiff_t difference_type;
	typedef std::hash<std::string> hasher;
	typedef std::equal_to<std::string> key_equal;
	typedef value &reference;
	typedef const value &const_reference;
	typedef value *pointer;
	typedef const value *const_pointer;
	typedef std::pair<std::string, value> container_value;
	typedef container_value *container_pointer;

	class array_iterator
	{
	public:
		array_iterator() : array_(nullptr), index_(0) {}
		array_iterator(container_pointer array, std::size_t index) : array_(array), index_(index) {}
		bool operator==(const array_iterator &comparand) const { return array_ == comparand.array_ && index_ == comparand.index_; }
		bool operator!=(const array_iterator &comparand) const { return !(*this == comparand); }
		array_iterator operator++(int) { array_iterator copy(array_, index_); index_++; return copy; }
		array_iterator &operator++() { index_++; return *this; }
		reference operator*() const { return array_[index_].second; }

	private:
		container_pointer array_;
		std::size_t index_;
	};

	class const_array_iterator
	{
	public:
		const_array_iterator() : array_(nullptr), index_(0) {}
		const_array_iterator(const container_pointer array, std::size_t index) : array_(array), index_(index) {}
		const_array_iterator &operator=(const const_array_iterator &) = delete;
		bool operator==(const const_array_iterator &comparand) const { return array_ == comparand.array_ && index_ == comparand.index_; }
		bool operator!=(const const_array_iterator &comparand) const { return !(*this == comparand); }
		const_array_iterator operator++(int) { const_array_iterator copy(array_, index_); index_++; return copy; }
		const_array_iterator &operator++() { index_++; return *this; }
		const_reference operator*() const { return array_[index_].second; }

	private:
		const container_pointer array_;
		std::size_t index_;
	};

	typedef std::reverse_iterator<array_iterator> reverse_array_iterator;
	typedef std::reverse_iterator<const_array_iterator> const_reverse_array_iterator;

	struct array_range
	{
		array_iterator first;
		array_iterator last;

		array_iterator begin() { return first; }
		array_iterator end() { return last; }
	};

	struct const_array_range
	{
		const_array_iterator first;
		const_array_iterator last;

		const_array_iterator begin() { return first; }
		const_array_iterator end() { return last; }
	};

	class object_iterator
	{
	public:
		object_iterator()
			: array_(nullptr), index_(0)
		{
		}

		object_iterator(container_pointer array, std::size_t index, std::size_t end)
			: array_(array), index_(index), end_(end)
		{
			advance();
		}

		bool operator==(const object_iterator &comparand) const
		{
			return array_ == comparand.array_ && index_ == comparand.index_;
		}

		bool operator!=(const object_iterator &comparand) const
		{
			return !(*this == comparand);
		}

		object_iterator operator++(int)
		{
			object_iterator copy(array_, index_, end_);
			index_++;
			advance();
			return copy;
		}

		object_iterator &operator++()
		{
			index_++;
			advance();
			return *this;
		}

		container_value &operator*() const
		{
			return array_[index_];
		}

		container_pointer operator->() const
		{
			return &array_[index_];
		}

	private:
		void advance()
		{
			if (array_ == nullptr)
			{
				return;
			}

			while (index_ < end_ && !array_[index_].second.is_valid())
			{
				index_++;
			}
		}

		container_pointer array_;
		std::size_t index_;
		std::size_t end_;
	};

	class const_object_iterator
	{
	public:
		const_object_iterator()
			: array_(nullptr), index_(0)
		{
		}

		const_object_iterator(const container_pointer array, std::size_t index, std::size_t end)
			: array_(array), index_(index), end_(end)
		{
			advance();
		}

		const_object_iterator &operator=(const const_object_iterator &) = delete;

		bool operator==(const const_object_iterator &comparand) const
		{
			return array_ == comparand.array_ && index_ == comparand.index_;
		}

		bool operator!=(const const_object_iterator &comparand) const
		{
			return !(*this == comparand);
		}

		const_object_iterator operator++(int)
		{
			const_object_iterator copy(array_, index_, end_);
			index_++;
			advance();
			return copy;
		}

		const_object_iterator &operator++()
		{
			index_++;
			advance();
			return *this;
		}

		const container_value &operator*() const
		{
			return array_[index_];
		}

		container_pointer operator->() const
		{
			return &array_[index_];
		}

	private:
		void advance()
		{
			if (array_ == nullptr)
			{
				return;
			}

			while (index_ < end_ && !array_[index_].second.is_valid())
			{
				index_++;
			}
		}

		const container_pointer array_;
		std::size_t index_;
		std::size_t end_;
	};

	typedef std::reverse_iterator<object_iterator> reverse_object_iterator;
	typedef std::reverse_iterator<const_object_iterator> const_reverse_object_iterator;

	struct object_range
	{
		object_iterator first;
		object_iterator last;

		object_iterator begin() { return first; }
		object_iterator end() { return last; }
	};

	struct const_object_range
	{
		const_object_iterator first;
		const_object_iterator last;

		const_object_iterator begin() { return first; }
		const_object_iterator end() { return last; }
	};

	// Member functions
	value() : type_(type::invalid_t), number_value_(0), container_(nullptr), size_(0), count_(0), max_load_factor_(0.75)
	{
	}

	value(value &&other) : value()
	{
		swap(*this, other);
	}

	value(const value &rhs) : value(rhs.type_)
	{
		size_ = rhs.size_;
		count_ = rhs.count_;
		max_load_factor_ = rhs.max_load_factor_;
		string_value_ = rhs.string_value_;
		number_value_ = rhs.number_value_;

        if(size_ > 0)
        {
            container_ = new container_value[size_];
#ifdef _WIN32
            auto last = stdext::checked_array_iterator<container_pointer>(container_, size_);
#else
            auto last = container_;
#endif
            std::copy(rhs.container_, rhs.container_ + rhs.size_, last);
        }
	}

	value(std::initializer_list<value> ilist) : value(type::array_t)
	{
		for (auto &v : ilist)
		{
			append(v);
		}
	}

	value(std::initializer_list<std::pair<std::string, value>> ilist) : value(type::object_t)
	{
		for (auto &v : ilist)
		{
			insert(v.first, v.second);
		}
	}

	value(bool b) : value(b ? type::true_t : type::false_t)
	{
	}

	value(int i) : value(type::number_t)
	{
		number_value_ = i;
	}

	value(double d) : value(type::number_t)
	{
		number_value_ = d;
	}

	value(const char *s) : value(type::string_t)
	{
		string_value_ = s;
	}

	value(const std::string &s) : value(type::string_t)
	{
		string_value_ = s;
	}

	~value()
	{
		if (container_ != nullptr)
		{
			delete[] container_;
		}
	}

	value copy() const
	{
		return value(*this);
	}

	value &operator=(value other)
	{
		swap(*this, other);
		return *this;
	}

	value &operator=(std::initializer_list<value> ilist)
	{
		value temp(ilist);
		swap(*this, temp);
		return *this;
	}

	bool is_literal() const { return is_null() || is_boolean(); }
	bool is_null() const { return type_ == type::null_t; }
	bool is_false() const { return type_ == type::false_t; }
	bool is_true() const { return type_ == type::true_t; }
	bool is_boolean() const { return is_true() || is_false(); }
	bool is_number() const { return type_ == type::number_t; }
	bool is_string() const { return type_ == type::string_t; }
	bool is_container() const { return is_object() || is_array(); }
	bool is_object() const { return type_ == type::object_t; }
	bool is_array() const { return type_ == type::array_t; }
	bool is_valid() const { return type_ != type::invalid_t; }

	std::string as_string() const { assert_type(type::string_t); return string_value_; }
	long double as_number() const { assert_type(type::number_t); return number_value_; }

	// Capacity
	bool empty() const
	{
		return size() == 0;
	}

	std::size_t size() const
	{
		if (is_container())
		{
			return count_;
		}

		throw std::runtime_error("invalid operation");
	}

	std::size_t max_size() const;

	// Modifiers
	void clear()
	{
		if (size_ > 0)
		{
			container_value blank = { "", value::invalid() };
			std::fill(container_, container_ + size_, blank);
		}

		count_ = 0;
	}

	template<class... Args>
	array_iterator emplace_array(value &&v)
	{
		assert_type(type::array_t);

		if (count_ + 1 > size_)
		{
			grow();
		}

		swap(container_[count_].second, v);
		return array_iterator(container_, count_++);
	}

	template<class... Args>
	array_iterator emplace_array_hint(const_array_iterator hint, Args&&... args);

	std::pair<object_iterator, bool> emplace_object(std::string &&key, value &&v)
	{
		using std::swap;

		assert_type(type::object_t);

		reserve(count_ + 1);

		size_type index = hash(key);
		size_type end = index == 0 ? size_ - 1 : index - 1;

		while (container_[index].second.is_valid() && !key_equal_(container_[index].first, key) && index != end)
		{
			index++;

			if (index == size_)
			{
				index = 0;
			}
		}

		bool was_occupied = true;

		if (!container_[index].second.is_valid())
		{
			was_occupied = false;
			count_++;
			swap(container_[index].second, v);
			swap(container_[index].first, key);
		}

		return std::pair<object_iterator, bool>(object_iterator(container_, index, size_), !was_occupied);
	}

	template<class... Args>
	object_iterator emplace_object_hint(const_object_iterator hint, Args&&... args);

	void erase(std::size_t index);

	void erase(const std::string &key);

	reference operator()(const value &v)
	{
		assert_type(type::array_t);
		append(v);
		return *this;
	}

	value operator()(const value &v) const
	{
		assert_type(type::array_t);
		value copy = *this;
		copy.append(v);
		return copy;
	}

	void append(const value &v)
	{
		assert_type(type::array_t);

		if (count_ + 1 > size_)
		{
			grow();
		}

		container_[count_++].second = v;
	}

	reference operator()(const std::string &key, const value &value)
	{
		assert_type(type::object_t);
		insert(key, value);
		return *this;
	}

	value operator()(const std::string &key, const value &v) const
	{
		assert_type(type::object_t);
		value copy = *this;
		copy.insert(key, v);
		return copy;
	}

	void insert(const std::string &key, const value &v)
	{
		assert_type(type::object_t);
		(*this)[key] = v;
	}

	// Lookup
	reference at(std::size_t index)
	{
		assert_type(type::array_t);

		if (index >= count_)
		{
			throw std::out_of_range("out of range");
		}

		return container_[index].second;
	}

	const_reference at(std::size_t index) const
	{
		assert_type(type::array_t);

		if (index >= count_)
		{
			throw std::out_of_range("out of range");
		}
		return container_[index].second;
	}

	reference at(const std::string &key)
	{
		assert_type(type::object_t);

		auto match = find(key);

		if (match == object_end())
		{
			throw std::out_of_range("no element found with specified key");
		}

		return match->second;
	}

	const_reference at(const std::string &key) const
	{
		assert_type(type::object_t);

		auto match = find(key);

		if (match == object_end())
		{
			throw std::out_of_range("no element found with specified key");
		}

		return match->second;
	}

	reference operator[](std::size_t index)
	{
		assert_type(type::array_t);
		return container_[index].second;
	}

	const_reference operator[](std::size_t index) const
	{
		assert_type(type::array_t);
		return container_[index].second;
	}

	reference operator[](const std::string &key)
	{
		assert_type(type::object_t);

		auto match = find(key);

		if (match != object_end())
		{
			return match->second;
		}

		reserve(count_ + 1);

		size_type index = hash(key);
		size_type end = index == 0 ? size_ - 1 : index - 1;

		while (container_[index].second.is_valid() && !key_equal_(container_[index].first, key) && index != end)
		{
			index++;

			if (index == size_)
			{
				index = 0;
			}
		}

		container_value &found = container_[index];

		if (!found.second.is_valid())
		{
			count_++;
			found = std::make_pair(key, value::null());
		}

		return found.second;
	}

	const_reference operator[](const std::string &key) const
	{
		assert_type(type::object_t);
		return at(key);
	}

	std::size_t count(const std::string &key) const;

	object_iterator find(const std::string &key)
	{
		if (size_ == 0)
		{
			return object_end();
		}

		auto index = hash(key);
		size_type end_index = index == 0 ? size_ - 1 : index - 1;

		while (container_[index].second.is_valid() && !key_equal_(container_[index].first, key) && index != end_index)
		{
			index++;

			if (index == size_)
			{
				index = 0;
			}
		}

		container_value &found = container_[index];

		if (found.second.is_valid() && key_equal_(found.first, key))
		{
			return object_iterator(container_, index, size_);
		}

		return object_end();
	}

	const_object_iterator find(const std::string &key) const
	{
		if (size_ == 0)
		{
			return object_end();
		}

		auto index = hash(key);
		size_type end_index = index == 0 ? size_ - 1 : index - 1;

		while (container_[index].second.is_valid() && !key_equal_(container_[index].first, key) && index != end_index)
		{
			index++;

			if (index == size_)
			{
				index = 0;
			}
		}

		if (container_[index].second.is_valid() && key_equal_(container_[index].first, key))
		{
			return const_object_iterator(container_, index, size_);
		}

		return object_end();
	}

	// Iteration interface
	array_range iter_array()
	{
		assert_type(type::array_t);
		return{ array_begin(), array_end() };
	}

	const_array_range iter_array() const
	{
		assert_type(type::array_t);
		return{ array_cbegin(), array_cend() };
	}

	object_range iter_object()
	{
		assert_type(type::object_t);
		return{ object_begin(), object_end() };
	}

	const_object_range iter_object() const
	{
		assert_type(type::object_t);
		return{ object_cbegin(), object_cend() };
	}

	bool operator!=(const value &rhs) const
	{
		return !(*this == rhs);
	}

	bool operator==(const value &rhs) const
	{
		if (type_ != rhs.type_)
		{
			return false;
		}

		switch (type_)
		{
		case type::null_t:
		case type::true_t:
		case type::false_t:
		case type::invalid_t:
            {
			return true;
            }
		case type::number_t:
            {
            auto difference = number_value_ - rhs.number_value_;
            if(difference < 0)
            {
                difference *= -1;
            }
			return difference < 0.0001;
            }
		case type::string_t:
            {
			return string_value_ == rhs.string_value_;
            }
		case type::object_t:
            {
			if (size_ != rhs.size_ || count_ != rhs.count_)
			{
				return false;
			}

			for (const auto &v : iter_object())
			{
				auto match = rhs.find(v.first);

				if (match == rhs.object_end() || match->second != v.second)
				{
					return false;
				}
			}
			break;
            }
		case type::array_t:
            {
			if (size_ != rhs.size_ || count_ != rhs.count_)
			{
				return false;
			}

			for (std::size_t i = 0; i < count_; i++)
			{
				if (at(i) != rhs.at(i))
				{
					return false;
				}
			}
			break;
            }
		}

		return true;
	}

	bool operator==(bool b) const { return (b && type_ == type::true_t) || (!b && type_ == type::false_t); }
	bool operator==(int i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(unsigned int i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(long i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(unsigned long i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(double i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(long double i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(long long i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(unsigned long long i) const { return type_ == type::number_t && number_value_ == i; }
	bool operator==(const std::string &s) const { return type_ == type::string_t && string_value_ == s; }
	bool operator==(const char *s) const { return (*this) == std::string(s); }

	friend std::ostream &operator<<(std::ostream &os, const value& v);

	friend void swap(value &first, value &second);

	static const value &null() { static value v(value::type::null_t); return v; }
	static const value &array() { static value v(value::type::array_t); return v; }
	static const value &object() { static value v(value::type::object_t); return v; }
	static const value &invalid() { static value v(value::type::invalid_t); return v; }

	// Iterators
	array_iterator array_begin()
	{
		return array_iterator(container_, 0);
	}

	const_array_iterator array_cbegin() const
	{
		return const_array_iterator(container_, 0);
	}

	array_iterator array_end()
	{
		return array_iterator(container_, count_);
	}

	const_array_iterator array_cend() const
	{
		return const_array_iterator(container_, count_);
	}

	object_iterator object_begin()
	{
		return object_iterator(container_, 0, size_);
	}

	const_object_iterator object_begin() const
	{
		return object_cbegin();
	}

	const_object_iterator object_cbegin() const
	{
		return const_object_iterator(container_, 0, size_);
	}

	object_iterator object_end()
	{
		return object_iterator(container_, size_, size_);
	}

	const_object_iterator object_end() const
	{
		return object_cend();
	}

	const_object_iterator object_cend() const
	{
		return const_object_iterator(container_, size_, size_);
	}

private:
	size_type hash(const std::string &key) const
	{
		return hasher_(key) % size_;
	}

	// Hash policy
	double load_factor() const
	{
		return count_ / (double)size_;
	}

	double max_load_factor() const
	{
		return max_load_factor_;
	}

	void max_load_factor(double z)
	{
		max_load_factor_ = z;
	}

	void rehash(size_type n)
	{
		if (n <= size_)
		{
			return;
		}

		auto old_size = size_;
		size_ = n;
		replace_container(container_, old_size, new container_value[n]);
	}

	void reserve(size_type n)
	{
		if (size_ == 0)
		{
			container_ = new container_value[initial_size];
			size_ = initial_size;
			return;
		}

		std::size_t required_size = (std::size_t)(n / max_load_factor());

		if (required_size <= size_)
		{
			return;
		}

		rehash(size_ * 2);
	}

	void replace_container(container_pointer source_container, size_type source_size, container_pointer replacement_container)
	{
		container_ = replacement_container;
		count_ = 0;

		for (std::size_t source_index = 0; source_index < source_size; source_index++)
		{
			if (!source_container[source_index].second.is_valid())
			{
				continue;
			}

			swap((*this)[source_container[source_index].first], source_container[source_index].second);
		}

		delete[] source_container;
	}

	void grow()
	{
		size_ = size_ == 0 ? initial_size : size_ * 2;
        container_pointer old = container_;
		container_ = new container_value[size_];

        if(count_ > 0 && old != nullptr)
        {
#ifdef _WIN32
            auto last = stdext::checked_array_iterator<container_pointer>(container_, size_);
#else
            auto last = container_;
#endif
            std::copy(old, old + count_, last);
        }

		delete[] old;
	}

	enum class type
	{
		invalid_t,
		null_t,
		false_t,
		true_t,
		number_t,
		string_t,
		array_t,
		object_t
	};

	value(type t) : value()
	{
		set_type(t);
	}

	void set_type(type t)
	{
		if (type_ == t)
		{
			return;
		}

		switch (type_)
		{
		case type::array_t:
			if (container_ != nullptr)
			{
				delete[] container_;
			}
			container_ = nullptr;
			size_ = 0;
			count_ = 0;
			break;
		case type::object_t:
			if (container_ != nullptr)
			{
				delete[] container_;
			}
			container_ = nullptr;
			size_ = 0;
			count_ = 0;
			break;
		case type::number_t:
			number_value_ = 0;
			break;
		case type::string_t:
			string_value_.clear();
			break;
		case type::true_t:
		case type::false_t:
		case type::null_t:
		case type::invalid_t:
			break;
		}

		type_ = t;
	}

	void assert_type(type t) const
	{
		if (type_ != t)
		{
			std::string type_string;

			switch (type_)
			{
			case type::null_t: type_string = "null"; break;
			case type::true_t: type_string = "true"; break;
			case type::false_t: type_string = "false"; break;
			case type::number_t: type_string = "number"; break;
			case type::string_t: type_string = "string"; break;
			case type::array_t: type_string = "array"; break;
			case type::object_t: type_string = "object"; break;
			case type::invalid_t: type_string = "invalid"; break;
			}

			throw std::runtime_error("operation not supported for type " + type_string);
		}
	}

	const std::size_t initial_size = 10;
	type type_;
	long double number_value_;
	std::string string_value_;
	container_pointer container_;
	size_type size_;
	size_type count_;
	hasher hasher_;
	key_equal key_equal_;
	double max_load_factor_;
};


inline void swap(value &first, value &second)
{
	using std::swap;

	swap(first.type_, second.type_);
	swap(first.string_value_, second.string_value_);
	swap(first.number_value_, second.number_value_);
	swap(first.container_, second.container_);
	swap(first.size_, second.size_);
	swap(first.count_, second.count_);
	swap(first.max_load_factor_, second.max_load_factor_);
}

} // namespace rana
