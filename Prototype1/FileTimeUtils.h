#pragma once

#include <windows.h>

inline bool operator < (const FILETIME& ft1, const FILETIME& ft2)
{
	return (::CompareFileTime(&ft1, &ft2) == -1);
}

inline bool operator > (const FILETIME& ft1, const FILETIME& ft2)
{
	return (::CompareFileTime(&ft1, &ft2) == 1);
}

inline bool operator == (const FILETIME& ft1, const FILETIME& ft2)
{
	return (::CompareFileTime(&ft1, &ft2) == 0);
}

inline bool operator != (const FILETIME& ft1, const FILETIME& ft2)
{
	return !(::CompareFileTime(&ft1, &ft2) == 0);
}