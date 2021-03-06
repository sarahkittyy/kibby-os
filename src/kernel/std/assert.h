#pragma once

#include "type.h"

#define assert(expr) m_assert(expr, #expr)

// internal assert function
void m_assert(bool res, const char* expr_str);
