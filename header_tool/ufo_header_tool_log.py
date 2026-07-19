
ufo_header_tool_log = ""

def ufo_header_tool_print(*_args):
    global ufo_header_tool_log

    print(*_args)

    for arg in _args:
        ufo_header_tool_log+=str(arg)+" "

    ufo_header_tool_log+="\n"
