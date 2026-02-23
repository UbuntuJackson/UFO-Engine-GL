import json
import sys


def add_actor(_working_directory, _counter, _parent_actor_name, _actor_json):
    code = ""

    print(_actor_json["name"])

    actor_name = "instance" + str(_counter.get_count())

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

    for content in structured_classes_dict["contents"]:
        if content["class"]["name"] == _actor_json["class_name"]:
            macros = content["macros"]
            klass = content["class"]
            print("Adding imported actor", _actor_json["class_name"], macros)

    this_actor_id = _counter.get_count()

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

    code += actors_string

    return code


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

    for content in structured_classes_dict["contents"]:
        spawner_function_string = ""

        klass = content["class"]
        macros = content["macros"]

        this_actor_id = actor_counter.get_count()

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
            "std::unique_ptr<"
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

        spawner_function_string += actors_string

        spawner_function_string += (
            "    return std::move(instance" + str(this_actor_id) + ");\n"
        )

        spawner_function_string += "}"

        generated_spawner_functions += spawner_function_string + "\n\n"

    print(generated_spawner_functions)

    return generated_spawner_functions
