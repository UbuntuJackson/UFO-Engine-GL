#pragma once

#if defined(__GNUC__) && defined(__cplusplus)
#define __UFO_PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif

#ifdef __MSC_VER
#define __UFO_PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define ufo_class()
#define ufo_category(_category)

#define ufo_variable()

#define ufo_int_range(_min, _max)
#define ufo_int_slider(_min,_max)

#define ufo_float_range(_min, _max)
#define ufo_float_slider(_min,_max, _step)

#define ufo_alias(_alias)

#define ufo_colour()

#define ufo_vector2f()

#define ufo_dropdown_menu(_e, ...)

#define ufo_radio_button(_e, ...)

#define ufo_ignore_file()

#define ufo_comment(_s)

#define ufo_method()

#define ufo_actor_config(_path)

//This is not used
#define ufo_garbage_collected()

#define UFO_CLASS()
#define UFO_CATEGORY(_category)

#define UFO_VARIABLE()

#define UFO_INT_RANGE(_min, _max)
#define UFO_INT_SLIDER(_min,_max)

#define UFO_FLOAT_RANGE(_min, _max)
#define UFO_FLOAT_SLIDER(_min,_max, _step)

#define UFO_ALIAS(_alias)

#define UFO_COLOUR()

#define UFO_VECTOR2f()

#define UFO_DROPDOWN_MENU(_e, ...)

#define UFO_RADIO_BUTTON(_e, ...)

#define UFO_IGNORE_FILE()

#define UFO_COMMENT(_s)

#define UFO_METHOD()

//This is not used
#define UFO_GARBAGE_COLLECTED()
