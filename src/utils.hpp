// Utility helpers for safe string handling.

#pragma once

#include <cstring>

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
inline void safe_strncpy(char (&dest)[N], const char *src) {
    static_assert(N > 0, "Destination buffer must not be empty");
    if (src == nullptr) {
        dest[0] = '\0';
        return;
    }

    // Copy at most N-1 bytes so we always have room for the terminator.
    std::strncpy(dest, src, N - 1);
    dest[N - 1] = '\0';
}

