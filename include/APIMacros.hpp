#pragma once

#include <Geode>

#ifdef GEODE_IS_WINDOWS
    #ifdef EXPORTING_GEODE_API
        #define GEODE_API_DLL __declspec(dllexport)
    #else
        #define GEODE_API_DLL __declspec(dllimport)
    #endif
#else
    #define GEODE_API_DLL
#endif
