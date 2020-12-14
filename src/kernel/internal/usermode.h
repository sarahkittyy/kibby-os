#pragma once

// calls a function in usermode.
extern void userexec(void (*fn)(void));
