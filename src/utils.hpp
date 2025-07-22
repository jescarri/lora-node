// Utility helpers for safe string handling.

#pragma once

#include <cstring>
#include <array>

// A small, constexpr-friendly wrapper around strncpy that guarantees
// null-termination of the destination buffer and avoids silent buffer
// overflows.
//
// Template parameter N is deduced from the destination array size, so the
// caller never needs to pass the length explicitly – this avoids the common
// off-by-one mistakes that make strncpy tricky to use correctly.
//
// Example:
//   char buf[16];
//   safe_strncpy(buf, source_c_str());
//
// After the call `buf` is always null-terminated, regardless of the length of
// `source_c_str()`.
template <std::size_t N>
inline void safe_strncpy(char (&dest)[N], const char* src) {
    static_assert(N > 0, "Destination buffer must not be empty");
    if (src == nullptr) {
        dest[0] = '\0';
        return;
    }

    // Copy characters one–by–one so we never read past the first null
    // terminator in `src` *and* we never write past the end of `dest`.
    // Using a simple loop avoids the well-known foot-guns of `strncpy` —
    // namely the need to remember the explicit "always terminate" step and
    // the unnecessary zero-padding it performs when the source is shorter
    // than the destination.

    std::size_t i = 0;
    for (; i < N - 1 && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }

    // Always terminate.
    dest[i] = '\0';
}

template <size_t N>
inline void safe_strncpy(std::array<char, N>& dest, const char* src) {
    static_assert(N > 0, "Destination buffer must not be empty");
    if (src == nullptr) {
        dest[0] = '\0';
        return;
    }
    std::size_t i = 0;
    for (; i < N - 1 && src[i] != '\0'; ++i) {
        dest[i] = src[i];
    }
    dest[i] = '\0';
}
