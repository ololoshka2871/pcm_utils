#pragma once

template<typename InputIt, typename Predicate>
static InputIt find_it_if(InputIt first, const InputIt &last, Predicate p) {
	while (first != last) {
		if (p(first))
			return first;
		++first;
	}
	return last;
}
