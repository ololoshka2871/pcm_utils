#pragma once

#include <initializer_list>

#include "Sample.h"

union CLASS_DECLSPEC DataBlock {
    enum class ElementNames {
        L0 = 0,
        R0 = 1,
        L1 = 2,
        R1 = 3,
        L2 = 4,
        R2 = 5,
        P = 6,
        Q = 7,

        COUNT = 8
    };

    DataBlock() = default;
    DataBlock(std::initializer_list<Sample> samples);

    Sample TR2() const;
    Sample T2L2() const;
    Sample T32R1() const;
    Sample T42L1() const;
    Sample T52R0() const;
    Sample T6L0() const;

    Sample Correct_P() const;
    Sample Correct_Q() const;

    Sample S_Pn() const;
    Sample S_Qn() const;

    const Sample& operator[](const ElementNames& element) const;
    Sample& operator[](const ElementNames& element);

    bool operator==(const DataBlock& lr);

    DataBlock& operator=(const DataBlock& lr);

    void* ptr();

    DataBlock Reversed() const;

    struct {
        Sample L0, R0, L1, R1, L2, R2, P, Q;
    } samples;
    Sample m[static_cast<size_t>(ElementNames::COUNT)];
};
