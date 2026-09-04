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

// Specify source file you would like to compile, not implemented yet.
#define ufo_compile(_source_file)

//This just makes it visible in the editor with no additions
#define ufo_class()

//Add actor to a category.
#define ufo_category(_category)

//Specify a component tree representing the actor. Ex: ufo_actor_config(src/player.ason)
#define ufo_actor_config(_path)

// Just an idea to define classes via an alias, like ufo_permanent_class_identifier(PlayerType)
#define ufo_permanent_class_identifier(_identifier)

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

//Unimplemented
#define ufo_vector2f()

//Unimplemented
#define ufo_dropdown_menu(_e, ...)

//Unimplemented
#define ufo_radio_button(_e, ...)

//Unimplemented but maybe useful
#define ufo_ignore_file()

//Adds a description to an exposed variable. Unimplemented.
#define ufo_comment(_s)

//Not implemented yet, but this macro would make it so you can have a ufo-class which doesn't show up in the editor but still has it's spawn-function generated
// in <project>/generated.h
#define ufo_hide_from_editor()

//Unimplemented but conceptually could be usefull to guard against parts you don't want to parse.
#define begin_parsing()
#define end_parsing()

//This is for a new feature I've been brainstorming that works more faithfully to godot script,
// I am uncertain how it will turn out. For now, just in case, use actors instead.

#define ufo_script()
