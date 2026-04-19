import json
import os.path
import sys

IMPORT_MODE_UNWRAPPED = 2
IMPORT_MODE_WRAPPED = 0


# For non-imported actor components, i.e components that don't have their own .ufo.h files and by extension no .ason file
def add_actor(_working_directory, _counter, _parent_actor_name, _actor_json):
    code = ""

    # The identifier of this component, will look something like instance1, instance2, instance300, etc.
    actor_name = "instance" + str(_counter.get_count())

    # Need to get the x and y values of course
    actor_position_x = _actor_json["x"]
    actor_position_y = _actor_json["y"]

    default_properties_string = ""

    default_properties_string += get_actor_loading_code(
        _actor_json["base_class_name"], actor_name, _actor_json
    )

    # Declaration of this component
    code += (
        "    auto "
        + actor_name
        + " = "
        + _parent_actor_name
        + "->AddActor<"
        + _actor_json["class_name"]
        + ">(Vector2f("
        + str(actor_position_x)
        + ","
        + str(actor_position_y)
        + "));\n"
    )

    # Have to set some metainfo such as editor_name and class_name. It could be useful at runtime.
    code += "    " + actor_name + '->editor_name = "' + _actor_json["name"] + '";\n'
    code += (
        "    " + actor_name + '->class_name = "' + _actor_json["class_name"] + '";\n'
    )

    code += default_properties_string

    # Recurse to generate more code for more components if there are any. These can also be imported.
    for actor in _actor_json["actors"]:
        if actor["import_mode"] == IMPORT_MODE_UNWRAPPED:
            code += add_actor(_working_directory, _counter, actor_name, actor)
        if actor["import_mode"] == IMPORT_MODE_WRAPPED:
            code += add_imported_actor(_working_directory, _counter, actor_name, actor)

    return code


def add_imported_actor(_working_directory, _counter, _parent_actor_name, _actor_json):

    code = ""

    structured_classes_file = open(_working_directory + "/structured_classes.json")

    structured_classes_dict = json.loads(structured_classes_file.read())

    structured_classes_file.close()

    macros = None
    klass = None

    this_actor_id = _counter.get_count()

    default_properties_string = ""

    for content in structured_classes_dict["contents"]:
        if content["class"]["name"] == _actor_json["class_name"]:
            macros = content["macros"]
            klass = content["class"]

            print("Adding imported actor", _actor_json["class_name"], macros)

    if klass == None:
        print(
            "add_imported_actor Error, klass is None", _parent_actor_name, _actor_json
        )
        sys.exit()

    # I'm considering using _actor_json to get the base_class_name. This code has been faulty and bottlenecking the toolchain by erroring
    # and not letting make_generated_file finish.

    default_properties_string += get_actor_loading_code(
        klass["extends"][0], "instance" + str(this_actor_id), _actor_json
    )

    custom_properties_string = ""

    # Assigning custom properties. These are a lot easier to deal with because the naming with the custom properties is a lot more consistent
    # than with default properties
    for key, custom_property in _actor_json["custom_editor_properties"].items():
        if custom_property["type"] == "std::string":
            custom_properties_string += (
                "    instance"
                + str(this_actor_id)
                + "->"
                + key
                + " = "
                + '"'
                + custom_property["value"]
                + '";\n'
            )

        if custom_property["type"] == "float":
            custom_properties_string += (
                "    instance"
                + str(this_actor_id)
                + "->"
                + key
                + " = "
                + str(custom_property["value"])
                + ";\n"
            )

        if custom_property["type"] == "int":
            custom_properties_string += (
                "    instance"
                + str(this_actor_id)
                + "->"
                + key
                + " = int("
                + str(custom_property["value"])
                + ")"
                + ";\n"
            )

        if custom_property["type"] == "bool":
            custom_properties_string += (
                "    instance"
                + str(this_actor_id)
                + "->"
                + key
                + " = bool("
                + str(custom_property["value"])
                + ")"
                + ";\n"
            )

    actor_name = "instance" + str(this_actor_id)

    actors_string = ""

    # Search for ufo_actor_config with path to .ason file. Same idea as in function main in this file, but with some modifications
    for macro in macros:
        if macro["name"] == "ufo_actor_config" and len(macro["args"]) > 0:
            actor_config_path = macro["args"][0]

            file_actor_config_file = open(_working_directory + "/" + actor_config_path)

            file_actor_config_json = json.loads(file_actor_config_file.read())

            main = None

            for i in file_actor_config_json["actors"]:
                if i["name"] == "Main":
                    main = i

            if not main:
                print(
                    "generate_actor_spawner_functions.py",
                    "Error, could not find Main actor",
                )
                sys.exit()

            # Iterate through all components of this actor
            for actor in main["actors"]:
                # If actor is not imported
                if actor["import_mode"] == IMPORT_MODE_UNWRAPPED:
                    actors_string += add_actor(
                        _working_directory,
                        _counter,
                        "instance" + str(this_actor_id),
                        actor,
                    )
                # If actor is imported
                if actor["import_mode"] == IMPORT_MODE_WRAPPED:
                    actors_string += add_imported_actor(
                        _working_directory,
                        _counter,
                        "instance" + str(this_actor_id),
                        actor,
                    )

            file_actor_config_file.close()

    actor_position_x = _actor_json["x"]
    actor_position_y = _actor_json["y"]

    code += (
        "    auto "
        + actor_name
        + " = "
        + _parent_actor_name
        + "->AddActor<"
        + _actor_json["class_name"]
        + ">(Vector2f("
        + str(actor_position_x)
        + ","
        + str(actor_position_y)
        + "));\n"
    )

    code += "    " + actor_name + '->editor_name = "' + _actor_json["name"] + '";\n'
    code += (
        "    " + actor_name + '->class_name = "' + _actor_json["class_name"] + '";\n'
    )

    code += default_properties_string

    code += custom_properties_string

    code += actors_string

    return code


# Generate code to set default properties for build-in class
def get_sprite_loading_code(_instance, _actor_json):
    default_properties_string = ""

    key = _actor_json["key"]
    offset_x = str(_actor_json["offset_x"])
    offset_y = str(_actor_json["offset_y"])
    frame_size_x = str(_actor_json["frame_size_x"])
    frame_size_y = str(_actor_json["frame_size_y"])
    scale_x = str(_actor_json["scale_x"])
    scale_y = str(_actor_json["scale_y"])
    rotation = str(_actor_json["rotation"])
    frame_index = str(_actor_json["frame_index"])

    default_properties_string += "    " + _instance + '->key = "' + key + '";\n'
    default_properties_string += "    " + _instance + "->offset.x = " + offset_x + ";\n"
    default_properties_string += "    " + _instance + "->offset.y = " + offset_y + ";\n"
    default_properties_string += (
        "    " + _instance + "->frame_size.x = " + frame_size_x + ";\n"
    )
    default_properties_string += (
        "    " + _instance + "->frame_size.y = " + frame_size_y + ";\n"
    )
    default_properties_string += "    " + _instance + "->scale.x = " + scale_x + ";\n"
    default_properties_string += "    " + _instance + "->scale.y = " + scale_y + ";\n"
    default_properties_string += "    " + _instance + "->rotation = " + rotation + ";\n"
    default_properties_string += (
        "    " + _instance + "->current_frame_index = " + frame_index + ";\n"
    )

    return default_properties_string


# Generate code to set default properties for build-in class
def get_animation_loading_code(_instance, _actor_json):
    default_properties_string = ""

    print("get_animation_loading_code", _actor_json["name"])

    for costume in _actor_json["costumes"]:
        key = costume["key"]
        offset_x = str(costume["offset_x"])
        offset_y = str(costume["offset_y"])
        frame_size_x = str(costume["frame_size_x"])
        frame_size_y = str(costume["frame_size_y"])
        scale_x = str(costume["scale_x"])
        scale_y = str(costume["scale_y"])
        rotation = str(costume["rotation"])
        frame_index = str(costume["frame_index"])
        animation_speed = str(costume["animation_speed"])

        default_properties_string += (
            "    "
            + _instance
            + "->AddCostume("
            + '"'
            + key
            + '"'
            + ","
            + "Vector2f("
            + "0.0f"
            + ","
            + "0.0f"
            + "),"
            + "Vector2f("
            + offset_x
            + ","
            + offset_y
            + "),"
            + "Vector2f("
            + frame_size_x
            + ","
            + frame_size_y
            + "),"
            + "Vector2f("
            + scale_x
            + ","
            + scale_y
            + "),"
            + rotation
            + ","
            + frame_index
            + ","
            + animation_speed
            + ");\n"
        )

    preview = str(_actor_json["preview"])

    default_properties_string += (
        "    " + _instance + "->preview = bool(" + preview + ");\n"
    )

    # Need to set the key because otherwise SetCostume will try to access the engine before AddNewActors, resulting in a crash
    default_properties_string += (
        "    " + _instance + '->key ="' + _actor_json["current_costume"] + '";\n'
    )

    return default_properties_string


# Generate code to set default properties for build-in class
def get_widget_loading_code(_instance, _actor_json):
    default_properties_string = ""

    rectangle = _actor_json["rectangle"]

    default_properties_string += (
        "    "
        + _instance
        + "->rectangle = ufo::Rectangle(Vector2f("
        + str(rectangle["x"])
        + ","
        + str(rectangle["y"])
        + "),Vector2f("
        + str(rectangle["w"])
        + ","
        + str(rectangle["h"])
        + "));\n"
    )

    return default_properties_string


# Generate code to set default properties for build-in class
def get_button_loading_code(_instance, _actor_json):
    default_properties_string = ""

    language_to_text = _actor_json["language_to_text"]

    for language, text in language_to_text.items():
        default_properties_string += (
            "    "
            + _instance
            + '->language_to_text["'
            + language
            + '"] = "'
            + text
            + '";\n'
        )

    default_properties_string += (
        "    "
        + _instance
        + "->is_wrapping = bool("
        + str(_actor_json["is_wrapping"])
        + ");\n"
    )

    return default_properties_string


def get_camera_loading_code(_instance, _actor_json):
    default_properties_string = ""

    scale = _actor_json["scale"]

    default_properties_string += "    " + _instance + "->scale = " + str(scale) + ";\n"

    clamp = _actor_json["clamp"]

    default_properties_string += (
        "    " + _instance + "->clamp = bool(" + str(clamp) + ");\n"
    )

    return default_properties_string


def get_tilemap_loading_code(_instance, _actor_json):
    return ""


def get_background_sprite_loading_code(_instance, _actor_json):
    return ""


def get_platformer_rectangle_collision_loading_code(_instance, _actor_json):
    return ""


def get_actor_loading_code(_base_class_name, _instance_, actor_json):
    if _base_class_name == "ufo::Sprite":
        return get_sprite_loading_code(_instance_, actor_json)

    if _base_class_name == "ufo::Animation":
        return get_animation_loading_code(_instance_, actor_json)

    if _base_class_name == "ufo::Widget":
        return get_widget_loading_code(_instance_, actor_json)

    if _base_class_name == "ufo::Button":
        return get_button_loading_code(_instance_, actor_json)

    if _base_class_name == "ufo::Camera":
        return get_camera_loading_code(_instance_, actor_json)

    return ""


# Entry point for this feature. This is called from function ufo_engine_header_tool.make_generated_file
def main(_working_directory):

    # Counter to generate unique identifiers present in <project>/generated.h, inside namespace ufo::Generated
    class ActorCounter:
        def __init__(self) -> None:
            self.count = 0

        def get_count(self):
            self.count += 1
            return self.count

    actor_counter = ActorCounter()

    # Load structured_classes.json to get all classes
    # if structured classes doesn't exist yet, then add nothing to generated code
    if not os.path.isfile(_working_directory + "/structured_classes.json"):
        return ""

    structured_classes_file = open(_working_directory + "/structured_classes.json")
    structured_classes_dict = json.loads(structured_classes_file.read())

    structured_classes_file.close()

    # This is part of generated.h and will end up after the custom actor generator ufo::Generated::ActorGenerator
    generated_spawner_functions = ""

    # Iterate through all classes
    for content in structured_classes_dict["contents"]:
        # This is the object that is returned by the spawner function
        this_actor_id = actor_counter.get_count()

        # This is the code for the actual spawner function
        spawner_function_string = ""

        # This string will contain the code loading the default properties for this actor, the actor returned by this function
        default_properties_string = ""

        # This json object represents the class.
        klass = content["class"]
        # These are all the macros and meta-settings like category, actor-config, and ufo-comments
        macros = content["macros"]

        # This string contains all components of this actor.
        actors_string = ""

        # Iterating through macros to find ufo_cator_config, and loading the .ason file taken as an argument
        for macro in macros:
            # Double checking that ufo_actor_config actually has an argument
            if macro["name"] == "ufo_actor_config" and len(macro["args"]) > 0:
                actor_config_path = macro["args"][0]

                file_actor_config_file = open(
                    _working_directory + "/" + actor_config_path
                )

                file_actor_config_json = json.loads(file_actor_config_file.read())

                # This is the actor named Main, in the .ason file.
                main = None

                # Looking for main-actor.
                for i in file_actor_config_json["actors"]:
                    if i["name"] == "Main":
                        main = i

                if not main:
                    print(
                        "[UFO-Engine Header Tool] generate_actor_spawner_functions.py",
                        "Error, could not find Main actor",
                    )
                    sys.exit()

                default_properties_string += get_actor_loading_code(
                    main["base_class_name"], "instance" + str(this_actor_id), main
                )

                # Evertything from here is just components, so there will be some loading of further .ason files
                # to get those default attributes too

                for actor in main["actors"]:
                    if actor["import_mode"] == IMPORT_MODE_UNWRAPPED:
                        actors_string += add_actor(
                            _working_directory,
                            actor_counter,
                            "instance" + str(this_actor_id),
                            actor,
                        )
                    if actor["import_mode"] == IMPORT_MODE_WRAPPED:
                        actors_string += add_imported_actor(
                            _working_directory,
                            actor_counter,
                            "instance" + str(this_actor_id),
                            actor,
                        )

                file_actor_config_file.close()

        klass_name = klass["name"]

        # Can't have colons in function names
        func_name = klass_name.replace("::", "_")

        # Function head
        spawner_function_string += (
            "inline std::unique_ptr<"
            + klass_name
            + ">"
            + " Spawn"
            + func_name
            + "(Vector2f _local_position){\n"
        )

        # Function body...
        spawner_function_string += (
            "    std::unique_ptr<"
            + klass_name
            + "> instance"
            + str(this_actor_id)
            + " = std::make_unique<"
            + klass_name
            + ">(_local_position);\n"
        )

        # runtime instantiated actor has no clas_name
        spawner_function_string += (
            "    instance"
            + str(this_actor_id)
            + '->class_name = "'
            + klass["name"]
            + '";\n'
        )

        spawner_function_string += default_properties_string

        spawner_function_string += actors_string

        spawner_function_string += (
            "    return std::move(instance" + str(this_actor_id) + ");\n"
        )

        spawner_function_string += "}"

        generated_spawner_functions += spawner_function_string + "\n\n"

        # Function body end.

    return generated_spawner_functions
