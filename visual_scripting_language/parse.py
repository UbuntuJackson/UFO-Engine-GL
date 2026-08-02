import pprint

def analyse_scopes(_file_contents_as_list, _scope, _line_number, _currently_searched_directory):
    separators = [";"]
    gibberish = ["\n", ";"]

    compound_object = []

    for index in range(len(_file_contents_as_list)):
        item = _file_contents_as_list[index]

        if item in separators:
            declared_scope = analyse_compound_object(compound_object, _line_number, _currently_searched_directory)
            if declared_scope is not None:
                _scope.scopes.append(declared_scope)

            compound_object.clear()

        if isinstance(item, list):
            declared_scope = analyse_compound_object(compound_object, _line_number, _currently_searched_directory)
            if declared_scope is not None:
                _scope.scopes.append(declared_scope)

                analyse_scopes(item, declared_scope, _line_number, _currently_searched_directory)

            compound_object.clear()
        else:
            if item not in gibberish:
                compound_object.append(item)

        if item == "\n":
            _line_number += 1


def detect_namespaces():
    pass


def detect_classes():
    pass


def remove_spaces(_file_contents_as_clean_list):
    space_less = []

    for i in _file_contents_as_clean_list:
        if i != " ":
            space_less.append(i)

    return space_less


def is_numeric_decimal_part(_f):
    if len(_f) > 0:
        if _f[-1] == "f":
            if _f[0:-1].isnumeric():
                return True
            else:
                return False
    else:
        return False


def clean_up_floating_point_numbers(_file_contents_as_list):
    compound_item = ""

    last_item = None

    separators = "={}(); ,+-*/%:\n#<>"

    for index in range(len(_file_contents_as_list)):
        item = _file_contents_as_list[index]

        if item == "." and (last_item is not None) and last_item.isnumeric():
            compound_item += last_item + "."
            _file_contents_as_list[index - 1] = ""
            _file_contents_as_list[index] = ""

        if (
            (last_item is not None)
            and last_item == "."
            and ((item.isnumeric()) or is_numeric_decimal_part(item))
        ):
            compound_item += item
            _file_contents_as_list[index - 1] = ""
            _file_contents_as_list[index] = ""

        if item is not None and item in separators and compound_item != "":
            _file_contents_as_list[index - 1] = compound_item
            compound_item = ""

        last_item = item

    clean_list = []
    for i in _file_contents_as_list:
        if i != "":
            clean_list.append(i)

    return clean_list


# Since comments can be confused for separators, the comment syntax // and /* */ should be made into their dedicated strings
def clean_up_multi_line_comments(_file_contents):
    former_item = None
    for index in range(len(_file_contents)):
        item = _file_contents[index]

        if former_item == "/" and item == "*":
            _file_contents[index - 1] = "/*"
            _file_contents[index] = ""
        # late phase

        if former_item == "*" and item == "/":
            _file_contents[index - 1] = "*/"
            _file_contents[index] = ""

        former_item = item

    clean_list = []
    for i in _file_contents:
        if i != "":
            clean_list.append(i)

    return clean_list


# Since comments can be confused for separators, the comment syntax // and /* */ should be made into their dedicated strings
def clean_up_single_line_comments(_file_contents):
    former_item = None
    for index in range(len(_file_contents)):
        item = _file_contents[index]

        if former_item == "/" and item == "/":
            _file_contents[index - 1] = "//"
            _file_contents[index] = ""
        # late phase

        former_item = item

    clean_list = []
    for i in _file_contents:
        if i != "":
            clean_list.append(i)

    return clean_list


# Some colons are supposed to be attached, for example std::string.
# Making them one string makes it a bit easier to parse
def clean_up_colons(_file_contents):
    former_item = None
    for index in range(len(_file_contents)):
        item = _file_contents[index]

        if former_item == ":" and item == ":":
            _file_contents[index - 1] = "::"
            _file_contents[index] = ""
        # late phase
        former_item = item

    clean_list = []
    for i in _file_contents:
        if i != "":
            clean_list.append(i)

    return clean_list


# This separates a c++ file in segments utilising a few characters as a guide
def separate_with_separators(_file_contents: str) -> list:
    separators = "={}(); ,+-*/%:\n#<>."

    words = []
    word = ""

    inside_string = False

    for i in _file_contents:
        # If stumble upon string-like object

        if inside_string and i != '"':
            word += i

        if i == '"':
            if not inside_string:
                inside_string = True
                word = ""
                words += i
                continue
            else:
                words.append(word)
                words += i
                word = ""
                inside_string = False
                continue

        if inside_string:
            continue

        if i in separators:
            if word != "":
                words.append(word)

            words.append(i)
            word = ""
            continue

        word += i

        if i in separators:
            words.append(word)
            word = ""

    return words

def get_contents_without_multi_line_comments(_file_contents_as_list):
    contents_without_comments = []

    inside_multi_line_comment = False

    for item in _file_contents_as_list:
        if not inside_multi_line_comment:
            if item == "/*":
                inside_multi_line_comment = True
                continue

        else:
            if item == "*/":
                inside_multi_line_comment = False
            continue

        contents_without_comments.append(item)

    return contents_without_comments


def get_contents_without_single_line_comments(_file_contents_as_list):
    contents_without_comments = []

    inside_single_line_comment = False

    for item in _file_contents_as_list:
        if not inside_single_line_comment:
            if item == "//":
                inside_single_line_comment = True
                continue

        else:
            if item == "\n":
                inside_single_line_comment = False
            continue

        contents_without_comments.append(item)

    return contents_without_comments


def detect_scopes(_file_contents_as_clean_list, _depth):
    squiggly_bracket_count = 0

    complete_scope = []
    scope_contents_temp = []

    for i in _file_contents_as_clean_list:
        if i == "{":
            should_continue = False
            if squiggly_bracket_count == 0:
                # ufo_header_tool_log.ufo_header_tool_print(_depth * "    " + "Scope start")
                should_continue = True

            squiggly_bracket_count += 1
            if should_continue:
                continue

        if i == "}":
            squiggly_bracket_count -= 1
            if squiggly_bracket_count == 0:
                # for j in scope_contents:
                #    ufo_header_tool_log.ufo_header_tool_print(_depth * "    " + "'" + j + "'")
                complete_scope.append(detect_scopes(scope_contents_temp, _depth + 1))

                # ufo_header_tool_log.ufo_header_tool_print(_depth * "    " + "Scope end")
                scope_contents_temp.clear()
                continue

        if squiggly_bracket_count != 0:
            scope_contents_temp.append(i)
        else:
            complete_scope.append(i)
            # ufo_header_tool_log.ufo_header_tool_print(_depth * "    " + "deep content '" + i + "'")

    return complete_scope

def main():
    f = open("player.us", 'r')

    file_contents = f.read()

    # Divide code with separators to make it a bit easier to parse
    segmented_file_contents = separate_with_separators(file_contents)

    # Some colons are supposed to be attached to eachother, like std::string or ufo::PlatformerCollision
    semented_file_contents_cleaned = clean_up_colons(segmented_file_contents)

    # Since comments can be confused for separators, the comment syntax // and /* */ should be made into their dedicated strings
    segmented_file_contents_cleaned_up_multi_line_comments = (
        clean_up_multi_line_comments(semented_file_contents_cleaned)
    )

    # Since comments can be confused for separators, the comment syntax // and /* */ should be made into their dedicated strings
    segmented_file_contents_cleaned_up_single_line_comments = (
        clean_up_single_line_comments(
            segmented_file_contents_cleaned_up_multi_line_comments
        )
    )

    #
    segmented_file_contents_single_line_comments_processed = (
        get_contents_without_single_line_comments(
            segmented_file_contents_cleaned_up_single_line_comments
        )
    )

    segmented_file_contents_multi_line_comments_processed = (
        get_contents_without_multi_line_comments(
            segmented_file_contents_single_line_comments_processed
        )
    )

    # Since floating point numbers contain separators, these need to be concatenated into dedicated strings
    segmented_file_contents_cleaned_up_floating_point_numbers = (
        clean_up_floating_point_numbers(
            segmented_file_contents_multi_line_comments_processed
        )
    )

    # We don't need spaces anymore, remove them.
    space_less_file_contents = remove_spaces(
        segmented_file_contents_cleaned_up_floating_point_numbers
    )

    f.close()

    scopes = detect_scopes(space_less_file_contents, 0)

    header_tool_file_log = pprint.pprint(scopes, indent=4)


main()
