import json
import pprint
import copy

def search_file(_header_file):
    print("Reading file:", _header_file)

    f = open(_header_file)
    
    word = ""

    class Word:
        def __init__(self, _word, _type_of_word):
            self.word = _word
            self.type_of_word = _type_of_word

    words = []

    separators = [" ", "\n", "{", "}", ",", '(', ')']
    
    NONE = -2
    BASE = -1
    STRING = 0
    CLASS = 1
    NAMESPACE = 2
    MACRO_CALL = 3
    FUNCTION_CALL = 4
    FUNCTION = 5
    FUNCTION_BODY = 6
    VARIABLE = 7
    MULTI_LINE_COMMENT = 8
    SINGLE_LINE_COMMENT = 9

    def check_inside(_value,_stack):
        for i in range(len(_stack)-1, -1, -1):
            if _stack[i].value == _value:
                return _stack[i]
        
        return None
    
    def pop_stack(_stack):
        member = _stack.pop()
        _stack[-1].members.append(member)

    class StackObject:
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []
        
        def print_tree(self, _index):
            print("   "*_index,self)
            print("   "*_index,"members of",self.name)
            for m in self.members:
                m.print_tree(_index+1)
            print("")
        
        def get_as_dictionary(self):
            return {
                "value" : self.value,
                "name" : self.name,
                "members" : [i.get_as_dictionary() for i in self.members]
            }
        
        def search(self, _pending_macro, _classes, _scope):

            for i in self.members:
                i.search(_pending_macro,_classes,"")
        
    class StackObjectNamespace(StackObject):
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []
        def search(self, _pending_macro, _classes, _scope):
            for i in self.members:
                i.search(_pending_macro,_classes,_scope+self.name+"::")
    
    class StackObjectClass(StackObject):
        def __init__(self, _value, _name, _parent_class, _header_file):
            self.value = _value
            self.name = _name
            self.parent_class = _parent_class
            self.header_file = _header_file
            self.members = []
        def search(self, _pending_macro, _classes, _scope):
            pending_macro = PendingMacro()
            #pending_macro.pending = True

            if _pending_macro.pending:
                _pending_macro.represents = {
                    "name":_scope+self.name,
                    "extends":self.parent_class,
                    "header_file" : self.header_file,
                    "members" : []
                }

                members = []

                for i in self.members:
                    i.search(pending_macro, members, _scope+self.name+"::")

                _pending_macro.represents["members"] = copy.deepcopy(members)

                _classes.append(copy.deepcopy(_pending_macro))
            
            _pending_macro.pending = False
            _pending_macro.clean_for_reuse()
            
    
    class StackObjectMacro(StackObject):
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []
        
        def search(self, _pending_macro, _classes, _scope):

            _pending_macro.pending = True
            _pending_macro.macros.append({self.name : [i.name for i in self.members]})
    
    class StackObjectString(StackObject):
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []
    
    class StackObjectFunction(StackObject):
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []

    class StackObjectFunctionBody(StackObject):
        def __init__(self, _value, _name):
            self.value = _value
            self.name = _name
            self.members = []
    
    class StackObjectVariable(StackObject):
        def __init__(self, _value, _name, _data_type, _variable_value):
            self.value = _value
            self.name = _name
            self.data_type = _data_type
            self.variable_value = _variable_value
            self.members = []
        def print_tree(self, _index):
            print(_index*"   ", self.data_type, self.name, self.variable_value)
        
        def search(self, _pending_macro,_member_variables, _scope):
            if _pending_macro.pending:

                _member_variables.append([_pending_macro.macros, {
                    "name" : self.name,
                    "data_type" : self.data_type,
                    "variable_value" : self.variable_value
                }])
            
            _pending_macro.clean_for_reuse()
            _pending_macro.pending = False

    stack = [StackObject(BASE, None)]

    for character in f.read():

        if stack[-1].value != MULTI_LINE_COMMENT:
            if word[len(word)-3 : len(word)-1] == "/*":
                stack.append(StackObject(MULTI_LINE_COMMENT,word))
                continue
        else:
            word+=character

            if word[len(word)-3 : len(word)-1] == "*/" :
                stack.pop()
                word = ""
            
            continue

        if stack[-1].value != SINGLE_LINE_COMMENT:
            if word[len(word)-3 : len(word)-1] == "//":
                stack.append(StackObject(SINGLE_LINE_COMMENT,word))
                continue
        else:
            word+=character

            if word[len(word)-2 : len(word)-1] == "\n" :
                stack.pop()
                word = ""
            
            continue

        #The strings gathered here are used for all string-like parameters including includes
        if stack[-1].value == STRING:

            #String end
            if character == '"':

                if stack[-2].value != MACRO_CALL: stack.pop()
                else:
                    stack[-1].name = word
                    pop_stack(stack)
                
                words.append(Word(word, STRING))
                word = ""
            else:
                word += character
            
            continue
        else:

            #String start
            if character == '"':
                if word != "": words.append(Word(word, NONE))
                
                stack.append(StackObjectString(STRING, word))
                word = ""
                
                continue
                

        #Checking for any object beginning with squiggly brackets
        if character == '{':
            if word != "": words.append(Word(word, NONE))
            word = ""

            if len(stack[-1].members) > 0 and stack[-1].members[-1].value == FUNCTION:
                stack.append(StackObjectFunctionBody(FUNCTION_BODY, None))
                words.append(Word(character, NONE))
                continue
            
            if words[-1].word == "namespace": stack.append(StackObjectNamespace(NAMESPACE, None))
            if words[-2].word == "namespace": stack.append(StackObjectNamespace(NAMESPACE, words[-1].word))

            if stack[-1].value == NAMESPACE:
                print("Found namespace", stack[-1].name)

            #No inheritence
            if words[-2].word == "class":
                stack.append(StackObjectClass(CLASS, words[-1].word, None))

            #Inheriting privately
            if words[-2].word == ":":
                if words[-4].word == "class": stack.append(StackObjectClass(CLASS, words[-3].word, words[-1].word, _header_file))

            #Inheriting publicly
            if words[-2].word == "public":
                if words[-5].word == "class": stack.append(StackObjectClass(CLASS, words[-4].word, words[-1].word, _header_file))

            #Just checking if a class was found.
            found_class = check_inside(CLASS, stack)

            if(found_class != None): print("Found class", found_class.name, "which inherits from", found_class.parent_class)

            words.append(Word(character, NONE))

            continue

        if character == '}' and (stack[-1].value == NAMESPACE or stack[-1].value == CLASS or stack[-1].value == FUNCTION_BODY):
            words.append(Word(character, NONE))
            pop_stack(stack)
        
        #Can be enabled later
        if character == ";":
            #Append whatever was before the semicolon
            if word != "": words.append(Word(word,NONE))
            word = ""

            #Make sure this isn't a forward declaration. Also need to check if the item before the semicolon
            # is a string litteral or not.
            if words[-2].word != 'class' and (words[-1].word not in [')', '}','{', '::', ':'] or words[-1].type_of_word == STRING):
                print("Found variable")
                if words[-2].word == '=':
                    stack.append(StackObjectVariable(VARIABLE,words[-3].word,words[-4].word,words[-1].word))
                    pop_stack(stack)
                    print("   ",words[-4].word,words[-3].word,words[-2].word,words[-1].word)
                else:
                    stack.append(StackObjectVariable(VARIABLE,words[-2].word,words[-3].word,None))
                    pop_stack(stack)
                    print("   ",words[-3].word,words[-2].word,words[-1].word)

            words.append(Word(character, NONE))
            word = ""
            continue

        if character == '(':

            #Append whatever was before the open parantheses
            if word != "": words.append(Word(word,NONE))
            word = ""

            #Just printing some info
            res_class = check_inside(CLASS, stack)
            res_namespace = check_inside(NAMESPACE, stack)
            if res_class and not res_namespace:
                print("Found macro, function or function-call",words[-1].word,"inside class",res_class.name)
            if res_class and res_namespace:
                print("Found macro, function or function-call",words[-1].word,"inside class",res_class.name, "inside namespace",res_namespace.name)
            if not res_class and not res_namespace:
                print("Found macro, function or function-call",words[-1].word)

            #So, macros tend to not have a datatype in front of them, but I'm not sure if
            # there are any catches to this
            # It could be worth brainstorming a way to account for function calls
            # function calls tend to not have function bodies and also usually end in
            # a semi-colon. macros can also end in semicolons however
            if words[-2].word in [';',':','{','}',')']:
                stack.append(StackObjectMacro(MACRO_CALL, words[-1].word))
            else:
                stack.append(StackObjectFunction(FUNCTION, words[-1].word))
            words.append(Word(character,NONE))
            continue
            
        #For some checks I wish to have the : in the list of words found
        if character == ":":

            if words[-1].word == character:
                words[-1].word+=character
                word = ""
                continue
            else:
                if word != "": words.append(Word(word, NONE))
                word = ""
                words.append(Word(character, NONE))
            
            continue

        if character in separators:
            
            if word != "":
                words.append(Word(word,NONE))

                if stack[-1].value == MACRO_CALL or stack[-1].value == FUNCTION:
                    stack[-1].members.append(StackObjectString(STRING, words[-1].word))
                    print("MACRO CALL", words[-1].word)

            word = ""

            
        else:
            word += character
        
        if character == ')' and (stack[-1].value == MACRO_CALL or stack[-1].value == FUNCTION):
            
            if word != "": words.append(Word(word,NONE))
            word = ""

            words.append(Word(character, NONE))
            pop_stack(stack)
    
    f.close()

    for w in words:
        print("'"+w.word+"'")
    
    if len(stack) == 1 and stack[0].value == BASE:
        print("Stack was popped properly :)")
        print(stack[0].print_tree(0))
    else:
        print("Error, stack elements were not properly stopped", stack)
    
    return stack

class PendingMacro:

    def __init__(self):
        self.pending = False
        self.macros = []
        self.represents = {}

    def clean_for_reuse(self):
        self.pending = False
        self.macros = []
        self.represents = {}

def make_generated_file(_classes):
    includes = "#include <functional>\n"
    includes+= "#include <memory>\n\n"
    includes+= '#include "UFO-Engine-GL/ufo_garbage_collector/gc_json.h"\n'
    includes+= '#include "UFO-Engine-GL/src/generic_generator.h"\n'
    includes+= '#include "UFO-Engine-GL/src/actor.h"\n'

    header_files = []

    for cl in _classes:
        if not cl.represents["header_file"] in header_files: header_files.append(cl.represents["header_file"])

    for i in header_files:
        includes += '#include "' + i+ '"\n'
    
    includes+="\n"

    namespace_string = "namespace Generated{\n\n"

    class_string = "class ActorGenerator : public ufo::GenericGenerator{\n\n"

    generator_map = "    std::map<std::string, std::function<std::unique_ptr<Actor>(ufo::gc::JsonMap* _json)>> factory_map;\n"

    function_ = "    void Initialise(){\n"

    for cl in _classes:
        function_ += "        factory_map.emplace(\n"
        
        function_ += '            "'+ cl.represents["name"] + '",\n'+"            [](ufo::gc::JsonMap* _json){\n"

        function_ += "                float _x = _json->map.at(\"x\")->AsFloat();\n"
        function_ += "                float _y = _json->map.at(\"y\")->AsFloat();\n"

        function_ += '                auto instance = std::make_unique<'+cl.represents["name"]+'>(Vector2f(_x, _y));\n'

        for member in cl.represents["members"]:
            if member[1]["data_type"] == 'int':
                function_ += '                instance->' +member[1]["name"]+ ' = (int)(_json->map.at(\"x\")->AsFloat());\n'
            if member[1]["data_type"] == 'float':
                function_ += '                instance->' +member[1]["name"]+ ' = _json->map.at(\"x\")->AsFloat();\n'
            if member[1]['data_type'] == 'Vector2f':
                pass
            if member[1]['data_type'] == 'string':
                function_ += '                instance->' +member[1]["name"]+ ' = _json->map.at(\"x\")->AsString();\n'

        function_ += "                return std::move(instance);\n"
        
        function_ += "            }\n        );\n"

    function_ += "    }\n"

    function_ += "    std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json){return std::move(factory_map.at(_json->map.at(\"name\")->AsString())(_json));}"

    function_ += "};\n\n"
    function_ += "}\n"

    f = open("generated.h", 'w')
    f.write(includes+ namespace_string+class_string+ generator_map +function_)
    f.close()


def main():
    #stack = search_file("UFO-Engine-GL/src/ufo_macros_example.h")

    stack2 = search_file("UFO-Engine-GL/src/enemy_example.h")

    dic = {
        "file_contents" : stack2[0].get_as_dictionary()
    }

    print(dic)

    s = json.dumps(dic, indent=4)
    f = open("parsed_file.json", "w")

    classes = []

    pending_macro = PendingMacro()

    stack2[-1].search(pending_macro, classes,"")

    print("############################")
    #pprint.pprint(classes)

    for cl in classes:
        pprint.pprint(cl.macros)
        pprint.pprint(cl.represents)

    make_generated_file(classes)

    f.write(s)

    f.close()

main()