#pragma once

#include <memory>
#include <string>
#include <dasm/Types.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#if !_DEBUG && DEBUG
#define PAUSE ;
#else
#ifndef PAUSE
#define PAUSE getchar();
#endif
#endif