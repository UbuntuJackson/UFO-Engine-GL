import json
import sys


def add_actor(_working_directory, _counter, _parent_actor_name, _actor_json):
    code = ""

    print(_actor_json["name"])

    actor_name = "instance" + str(_counter.get_count())

    actor_position_x = _actor_json["x"]
    actor_position_y = _actor_json["y"]

    default_properties_string = ""

    if _actor_json["base_class_name"] == "ufo::Sprite":
        default_properties_string += get_sprite_loading_code(actor_name, _actor_json)

    if _actor_json["base_class_name"] == "ufo::Animation":
        default_properties_string += get_animation_loading_code(actor_name, _actor_json)

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

    code += default_properties_string

    for actor in _actor_json["actors"]:
        if actor["import_mode"] == 2:
            code += add_actor(_working_directory, _counter, actor_name, actor)
        if actor["import_mode"] == 0:
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

    if klass["extends"][0] == "ufo::Sprite":
        default_properties_string += get_sprite_loading_code(
            "instance" + str(this_actor_id), _actor_json
        )

    if klass["extends"][0] == "ufo::Animation":
        default_properties_string += get_animation_loading_code(
            "instance" + str(this_actor_id), _actor_json
        )

    custom_properties_string = ""

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

    for macro in macros:
        if macro["name"] == "ufo_actor_config" and len(macro["args"]) > 0:
            actor_config_path = macro["args"][0]

            file_actor_config_file = open(_working_directory + "/" + actor_config_path)

            file_actor_config_json = json.loads(file_actor_config_file.read())

            main = None

            for i in file_actor_config_json["actors"]:
                if i["name"] == "Main":
                    main = i

            if main:
                actors = main["actors"]
            else:
                print(
                    "generate_actor_spawner_functions.py",
                    "Error, could not find Main actor",
                )
                sys.exit()

            for actor in actors:
                if actor["import_mode"] == 2:
                    actors_string += add_actor(
                        _working_directory,
                        _counter,
                        "instance" + str(this_actor_id),
                        actor,
                    )
                if actor["import_mode"] == 0:
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

    code += default_properties_string

    code += custom_properties_string

    code += actors_string

    return code


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

    # Need to set the key because otherwise SetCostume will try to access the engine before AddNewActors, resulting in a crash
    default_properties_string += (
        "    " + _instance + '->key ="' + _actor_json["current_costume"] + '";\n'
    )

    return default_properties_string


def main(_working_directory):

    class ActorCounter:
        def __init__(self) -> None:
            self.count = 0

        def get_count(self):
            self.count += 1
            return self.count

    actor_counter = ActorCounter()

    structured_classes_file = open(_working_directory + "/structured_classes.json")

    structured_classes_dict = json.loads(structured_classes_file.read())

    print(structured_classes_dict)

    structured_classes_file.close()

    generated_spawner_functions = ""

    this_actor_id = actor_counter.get_count()

    for content in structured_classes_dict["contents"]:
        spawner_function_string = ""

        default_properties_string = ""

        klass = content["class"]
        macros = content["macros"]

        actors_string = ""

        for macro in macros:
            if macro["name"] == "ufo_actor_config" and len(macro["args"]) > 0:
                actor_config_path = macro["args"][0]

                file_actor_config_file = open(
                    _working_directory + "/" + actor_config_path
                )

                file_actor_config_json = json.loads(file_actor_config_file.read())

                main = None

                for i in file_actor_config_json["actors"]:
                    if i["name"] == "Main":
                        main = i

                if main:
                    actors = main["actors"]
                else:
                    print(
                        "generate_actor_spawner_functions.py",
                        "Error, could not find Main actor",
                    )
                    sys.exit()

                if klass["extends"][0] == "ufo::Sprite":
                    default_properties_string += get_sprite_loading_code(
                        "instance" + str(this_actor_id), main
                    )

                if klass["extends"][0] == "ufo::Animation":
                    default_properties_string += get_animation_loading_code(
                        "instance" + str(this_actor_id), main
                    )

                for actor in actors:
                    if actor["import_mode"] == 2:
                        actors_string += add_actor(
                            _working_directory,
                            actor_counter,
                            "instance" + str(this_actor_id),
                            actor,
                        )
                    if actor["import_mode"] == 0:
                        actors_string += add_imported_actor(
                            _working_directory,
                            actor_counter,
                            "instance" + str(this_actor_id),
                            actor,
                        )

                file_actor_config_file.close()

        klass_name = klass["name"]

        func_name = klass_name.replace("::", "_")

        spawner_function_string += (
            "inline std::unique_ptr<"
            + klass_name
            + ">"
            + " Spawn"
            + func_name
            + "(Vector2f _local_position){\n"
        )

        spawner_function_string += (
            "    std::unique_ptr<"
            + klass_name
            + "> instance"
            + str(this_actor_id)
            + " = std::make_unique<"
            + klass_name
            + ">(_local_position);\n"
        )

        spawner_function_string += default_properties_string

        spawner_function_string += actors_string

        spawner_function_string += (
            "    return std::move(instance" + str(this_actor_id) + ");\n"
        )

        spawner_function_string += "}"

        generated_spawner_functions += spawner_function_string + "\n\n"

    return generated_spawner_functions
