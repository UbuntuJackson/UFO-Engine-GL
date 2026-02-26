import json
import sys


def main(_working_directory, _path):
    tiled_map_file = open(_working_directory + "/" + _path, "r")

    print(
        "[UFO-Engine Studio] Running Tiled Map Conversion Tool, (UFO-Engine/tiled_map_conversion_tool/tiled_map_conversion_tool.py)\n    ",
        _working_directory + "/" + _path,
    )

    tiled_map_dict = json.loads(tiled_map_file.read())

    tiled_map_file.close()

    layers = tiled_map_dict["layers"]
    tilesets = tiled_map_dict["tilesets"]

    ufo_engine_level_file = {
        "actors": [],
        "base_class_name": "ufo::Level",
        "class_name": "ufo::Level",
        "custom_editor_properties": {},
        "import_mode": 2,
        "is_imported": 0,
        "name": "@Instanceufo::Actor4",
        "size_x": 20000,
        "size_y": 3000,
        "tilesets": [],
        "x": 0,
        "y": 0,
    }

    for layer in layers:
        if layer["type"] != "tilelayer":
            continue

        empty_tilemap = {
            "actors": [],
            "base_class_name": "ufo::TileMap",
            "class_name": "ufo::TileMap",
            "custom_editor_properties": {},
            "import_mode": 2,
            "is_imported": 0,
            "name": "ufo::TileMap248",
            "number_of_columns": layer["width"],
            "number_of_rows": layer["height"],
            "tiles": layer["data"],
            "visible": 1,
            "x": 0,
            "y": 0,
        }

        ufo_engine_level_file["actors"].append(empty_tilemap)

    for tileset in tilesets:
        tiled_tileset = {
            "columns": 11,
            "firstgid": 1,
            "image": "..\/gfx\/icecavetiles.png",
            "imageheight": 288,
            "imagewidth": 176,
            "margin": 0,
            "name": "icecave",
            "spacing": 0,
            "tilecount": 198,
            "tileheight": 16,
            "tilewidth": 16,
        }

        ufo_tileset = {
            "columns": tileset["columns"],
            "image_height": tileset["imageheight"],
            "image_width": tileset["imagewidth"],
            "name": tileset["image"],
            "tile_count": tileset["tilecount"],
            "tile_height": tileset["tileheight"],
            "tile_width": tileset["tilewidth"],
            "tileset_start_id": tileset["firstgid"],
        }

        ufo_engine_level_file["tilesets"].append(ufo_tileset)

    out_file_contents = json.dumps(ufo_engine_level_file)

    out_file = open(
        _working_directory
        + "/"
        + _path.replace(".json", "_converted_from_tiled")
        + ".ason",
        "w",
    )

    out_file.write(out_file_contents)
    out_file.close()


main(sys.argv[1], sys.argv[2])
