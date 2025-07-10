// Minimal substitute for AVR <pgmspace.h> on the host – simply maps the
// program-memory copy routine to the standard memcpy.

#pragma once

#include <cstring>

#define memcpy_P(dest, src, len) std::memcpy((dest), (src), (len))

