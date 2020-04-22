#pragma once


template<typename ValueType>
class OwnIterator_Base : public std::iterator<std::input_iterator_tag, ValueType>
{
public:
	bool operator!=(OwnIterator_Base const& other) const {
		return p != other.p;
	}

	bool operator==(OwnIterator_Base const& other) const {
		return p == other.p;
	}

	typename OwnIterator_Base::reference operator*() const {
		return *p;
	}

	bool valid() const {
		return (p >= start) && (p < _end);
	}

	auto start_offset() const {
		return p - start;
	}

protected:
	OwnIterator_Base(ValueType* p, ValueType *start, ValueType *end) : p(p), start(start), _end(end) {}
	OwnIterator_Base(const OwnIterator_Base &it) : p(it.p), start(it.start), _end(it._end) { }

	ValueType* p;
	ValueType *start, *_end;
};

template<typename ValueType>
class OwnForwardIterator : public OwnIterator_Base<ValueType>
{
public:
	using this_type = OwnForwardIterator<ValueType>;

	OwnForwardIterator(ValueType* p, size_t max_size) : OwnIterator_Base<ValueType>(p, p, p + max_size) {}
	OwnForwardIterator(ValueType* p, ValueType *start, ValueType *end) : OwnIterator_Base<ValueType>(p, start, end) {}

	this_type& operator++() {
		++this->p;
		return *this;
	}

	this_type next() const {
		return this_type(this->p + 1, this->_end - this->p);
	}

	this_type End() const {
		return this_type(this->_end, this->start, this->_end);
	}
};

template<typename ValueType>
class OwnReversIterator : public OwnIterator_Base<ValueType>
{
public:
	OwnReversIterator(ValueType* p, size_t max_size) : OwnIterator_Base<ValueType>(p + max_size - 1, p, p + max_size) {}
	OwnReversIterator(ValueType* p, ValueType *start, ValueType *end) : OwnIterator_Base<ValueType>(p, start, end) {}

	OwnReversIterator& operator++() {
		--this->p;
		return *this;
	}

	OwnReversIterator next() {
		return OwnReversIterator(this->p - 1, this->start, this->p);
	}

	OwnReversIterator End() const {
		return OwnReversIterator(this->start - 1, this->start - 1, this->_end);
	}
};
