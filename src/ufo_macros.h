//These macros expand to nothing, and are only used for The UFO-Header Tool & Boilerplate Generator.
// Hence it seems a bit nonsensical and useless at first, but it's actually for external parsing.

#pragma once

#if defined(__GNUC__) && defined(__cplusplus)
#define __UFO_PRETTY_FUNCTION__ __PRETTY_FUNCTION__
#endif

#ifdef __MSC_VER
#define __UFO_PRETTY_FUNCTION__ __FUNCSIG__
#endif

//These macros exposes your actor as spawnable in UFO-Engine Studio.
#define ufo_class()

//Add actor to a category.
#define ufo_category(_category)

//Specify a component tree representing the actor. Ex: ufo_actor_config(src/player.ason)
#define ufo_actor_config(_path)
// ?
#define permanent_class_identifier(_identifier)

//These macros are for member variables of an actor exposed to UFO-Engine Studio.
//ufo_variable just exposes the variable with no modifications.
#define ufo_variable()

//For interval based values like sliders. Assign variable with equal operator to set a default value, ex. ufo_int_range(0,20) int number = 15;
#define ufo_int_range(_min, _max)
#define ufo_int_slider(_min,_max)

#define ufo_float_range(_min, _max)
#define ufo_float_slider(_min,_max, _step)

//Sets the display name in the editor.
#define ufo_alias(_alias)

#define ufo_colour()

#define ufo_vector2f()

#define ufo_dropdown_menu(_e, ...)

#define ufo_radio_button(_e, ...)

#define ufo_ignore_file()

//Adds a description to an exposed variable. Unimplemented.
#define ufo_comment(_s)

//Unimplemented, not sure what a ufo_method would mean.
#define ufo_method()

//This is not used
#define ufo_garbage_collected()

//Unimplemented but conceptually could be usefull to guard against parts you don't want to parse.
#define begin_parsing()
#define end_parsing()

//Allcaps versions of the above macros. Not valid yet.
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
