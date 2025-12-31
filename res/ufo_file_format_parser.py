import pprint


def main():
    separators = ["\n", " ", "&", "[", "]", "#"]

    f = open("level_format_brainstorming.txt")

    words = []

    word = ""

    for i in f.read():
        if i in separators:
            words.append(word)
            words.append(i)
            word = ""
        else:
            word += i

    f.close()

    words_without_comments = []

    found_comment = False

    for i in words:
        if i == "\n":
            found_comment = False
            continue
        if i == "#":
            found_comment = True
            continue

        if not found_comment:
            words_without_comments.append(i)

    words_without_spaces = []

    for i in words_without_comments:
        if i not in [" ", "\n", ""]:
            words_without_spaces.append(i)

    for i in words_without_spaces:
        print("'" + i + "'")

    variables = []

    variable = []

    for i in words_without_spaces:
        if len(variable) > 1 and i not in separators:
            if not i.isnumeric() or not variable[-1].isnumeric():
                variables.append(variable)
                variable.clear()

        variable.append(i)

    variables.append(variable)

    pprint.pprint(variables)


main()
