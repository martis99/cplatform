#ifndef PDEF_H
#define PDEF_H

#include "platform.h"

#if defined(C_WIN) && defined(PLT_BUILD_DLL)
	#define PLTAPI __declspec(dllexport)
#elif defined(C_WIN) && defined(PLT_DLL)
	#define PLTAPI __declspec(dllimport)
#elif defined(C_LINUX) && defined(PLT_BUILD_DLL)
	#define PLTAPI __attribute__((visibility("default")))
#else
	#define PLTAPI
#endif
#endif
