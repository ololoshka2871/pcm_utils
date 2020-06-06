#pragma once

#include <cassert>
#include <cstdint>

template <typename T> class myTwoDimArray {
public:
  myTwoDimArray(void *data, const int32_t width, const int32_t heigth)
      : data(reinterpret_cast<T *>(data)), width(width), heigth(heigth) {}

  void setData(T *newptr) { data = newptr; }

  T &element(int32_t x, int32_t y) {
    assert(x < width && y < heigth);
    return getLine(y)[x];
  }

  int32_t getwidth() const { return width; }

  int32_t getheigth() const { return heigth; }

  T *getLine(int32_t y) const {
    assert(y < heigth);
    return &data[y * width];
  }

private:
  T *data;
  const int32_t width, heigth;
};
