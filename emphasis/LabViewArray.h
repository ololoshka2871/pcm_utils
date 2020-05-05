#pragma once

template <typename T>
struct LabViewArray {
	UINT32 Size;
	T elements[];

	LabViewArray(const LabViewArray& lr) : Size(lr.Size) {
		std::memcpy(elements, lr.elements, lr.Size);
	}

	LabViewArray& operator=(const LabViewArray& lr) {
		Size = lr.Size;
		std::memcpy(elements, lr.elements, lr.Size);
		return *this;
	}
};