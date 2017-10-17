#pragma once

#define COND(cond) cond
#define ASSERT(cond, text) if (COND(cond)) {} else {__debugbreak(); printf(text);}