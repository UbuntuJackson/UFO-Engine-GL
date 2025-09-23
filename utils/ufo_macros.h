#pragma once

#if defined(__GNUC__) && defined(__cplusplus)
#define __UFO_PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif

#ifdef __MSC_VER
#define __UFO_PRETTY_FUNCTION__ __FUNCSIG__
#endif