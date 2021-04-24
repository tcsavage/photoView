#pragma once

template <class T>
struct SemigroupTraits {};

struct Sum;

template <class T>
struct SemigroupTraits {
    static T append(const T &lhs, const T &rhs) noexcept;
};
