#pragma once
#ifdef WCONTOUR_EXPORTS
#define WCONTOUR_API __declspec(dllexport)
#else
#define WCONTOUR_API __declspec(dllimport)
#endif
