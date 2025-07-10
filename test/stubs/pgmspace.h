// Minimal substitute for AVR <pgmspace.h> on the host – simply maps the
// program-memory copy routine used by the CayenneLPP library to the standard
// memcpy available on every platform.

#pragma once

#include <cstring>

#define memcpy_P(dest, src, len) std::memcpy((dest), (src), (len))

