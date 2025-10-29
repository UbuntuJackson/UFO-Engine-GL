import sys

class Attribute:
    def __init__(self):
        self.name = None
        self.values = []

class Variable:
    datatypes = [
        'int',
        'float',
        'bool',
        'String'
    ]

    def __init__(self):
        self.data_type = None
        self.attributes = []
        self.default_value = None
        self.name = None
        self.found_initialisation = False

#UNUSED
def evaluate_variable_line(_word):
    is_in_string = False
    
    trimmed_string = ""

    word = ""

    for character in _word:
        if character == '"' and not is_in_string:
            is_in_string = True
        else:
            is_in_string = False
        
        if not is_in_string:
            trimmed_string += character
    
    for character in trimmed_string:
        if character == '':
            pass


#Multiline comments are evaluated to single line comments
def evaluate_comment(_log, _comment, _line_count):
    word = ""
    is_attributes = False

    attributes = [Attribute()]
    awaiting_arguments = False

    for character in _comment:

        if not is_attributes:

            word += character
        

            if word == '@attributes':
                is_attributes = True
                word = ""
            elif len(word) == len('@attributes'):
                #print(word)
                return []

            continue

        if awaiting_arguments:
            if character in [',', ';']:
                #Time to evaluate what datatype the argument is

                ttype = None
                if len(word) > 1:
                    if word[0] == '"' and word[-1] == '"':
                        ttype = "String"
                        word = word[:-1]
                        word = word [1:]
                        #_log.append("word was cut:", word)
                
                if ttype != 'String':
                    found_dot = False
                    is_not_numerical = False
                    first_round = False

                    for i in word:
                        if i == '-' and first_round:
                            _log.append("UFO Project Manager: Syntax error at line: x", "No mathmatical expressions allowed in UPM attributes "+ word)
                            return []
                        first_round = True
                        
                        if i not in '1234567890.-':
                            is_not_numerical = True
                            #print("UFO Project Manager: Syntax error at line:",_line_count , "Not a number",word)
                            #sys.exit()
                            #break
                        if i == '.':
                            found_dot = True
                            continue
                        if i == '.' and found_dot:
                            _log.append("UFO Project Manager: Syntax error at line: " + str(_line_count) + "Too comma signs in number " + word)
                            return []
                    
                    if not found_dot and not is_not_numerical:
                        ttype = 'int'
                    if found_dot and not is_not_numerical:
                        ttype = 'float'
                    
                    if is_not_numerical:
                        if word == 'true' or word == 'false':
                            ttype = 'bool'
                        else:
                            _log.append("[x] UPM Header Tool: Error, argument of unknown datatype "+ word + " at line: "+ str(_line_count))

                attributes[-1].values.append((ttype,word))
                word = ""
            else:
                word += character
            
            if character == ';':
                awaiting_arguments = False
                attributes.append(Attribute())
                word = ""
            continue
        
        if character not in ['=', ':']:
            word += character
        else:
            
            attributes[-1].name = word
            awaiting_arguments = True
            _log.append("---------------------------------------------")
            _log.append("Found name: "+ word)
            word = ""

        continue

    #Due to how I do things, there is an extra Attribute after each semicolon
    attributes.pop()

    for i in attributes:
        _log.append("-----------------------------------")
        _log.append("Name: "+i.name)
        _log.append("Values: "+str(i.values))
    
    return attributes

def export_variables(_log, _source_file):
    variables_to_export = []
    line_count : int = 1

    is_in_string : bool = False
    multi_line_ender_progress = ""
    comment_ender = None
    awaiting_comment = False
    completed_single_line_comment = False
    currently_evaluated_attributes = None
    line_number_before_variable = 0

    word = ""

    variable_line = ""
    
    f = open(_source_file, 'r')

    file_string : str = f.read()

    for character_index in range(0, len(file_string)):
        character = file_string[character_index]

        if character_index >= 1 :
            if file_string[character_index-1] == '\n': line_count += 1

        if comment_ender == '\n' and character != '\n':
            word += character
            completed_single_line_comment = True
            continue

        if awaiting_comment:
            if character != '\n': word += character

            if len(multi_line_ender_progress) > 0 and character != '/': multi_line_ender_progress = ""

            if character == '*':
                #_log.append("Found *")
                multi_line_ender_progress += '*'

            if multi_line_ender_progress == '*' and character == '/':
                
                awaiting_comment = False
                #_log.append("Found end of multi line comment "+word.strip()[:-2]+' end')
                word = word.strip()[:-2]
                multi_line_ender_progress = ''
                completed_single_line_comment = True
            
            if not completed_single_line_comment: continue

        if character == '\n' and completed_single_line_comment:
            completed_single_line_comment = False
            comment_ender = None

            inside_string_do_not_replace_spaces : str = False

            stripped_string : str = ""

            for index in range(len(word)):
                if word[index] == '"':
                    if not inside_string_do_not_replace_spaces:
                        inside_string_do_not_replace_spaces = True
                    else:
                        inside_string_do_not_replace_spaces = False
                
                if (not (word[index] in [' ', '\n'])) or inside_string_do_not_replace_spaces:
                    stripped_string+=word[index]
            
            #print(stripped_string)
            currently_evaluated_attributes = evaluate_comment(_log,stripped_string, line_count)

            #Was there an attribute list?
            if len(currently_evaluated_attributes) == 0:
                currently_evaluated_attributes = None
            else:

                line_number_before_variable = line_count

                variables_to_export.append(Variable())

            print("End Comment:", word, "at line", line_count)

        if character == '"' and currently_evaluated_attributes == None:
            if not is_in_string:
                #print("Start String")
                is_in_string = True
            else:
                #print("End String")
                is_in_string = False
            word = ""
            continue

        if is_in_string: continue

        if word == '/*':
            awaiting_comment = True
            word = ""

        if word == '//':
            comment_ender = '\n'
            _log.append("Begin comment at line " + str(line_count))
            word = ""

        if currently_evaluated_attributes != None and line_count == line_number_before_variable+1:

            if variables_to_export[-1].data_type == 'class' and variables_to_export[-1].default_value == None:
                if variables_to_export[-1].name != None:
                    if character in ['\n', '{']:
                        variables_to_export[-1].default_value = word
                        #print(word)
                        currently_evaluated_attributes = None

            #Check if there is a base class that is inherited from
            if (variables_to_export[-1].data_type == 'class' and character in ['\n', '{']):
                if variables_to_export[-1].default_value == None:
                    _log.append("[x] UPM Header Tool: Error, class needs to inherit from UFO-Engine class" + str(line_count))
                    return []

            #Evaluating whether or not there is a variable to attach attributes to
            if character in [' ', '\n', ';', '=']:
                #print("line_number_before_variable", line_number_before_variable, "'"+word+"'")
                #sys.exit()                

                if character == '=':
                    variables_to_export[-1].found_initialisation = True

                if variables_to_export[-1].name != None:

                    if character in[';', '\n']:
                        currently_evaluated_attributes = None
                        if not variables_to_export[-1].found_initialisation:
                            _log.append("[x] UPM Header Tool: Error, no default value was set at line" + str(line_count))
                            return []
                    
                    if variables_to_export[-1].found_initialisation:
                        if word == '': continue

                        if variables_to_export[-1].data_type == 'std::string':
                            word = word[:-1]
                            word = word [1:]
                        variables_to_export[-1].default_value = word
                        _log.append(variables_to_export[-1].data_type + " " + variables_to_export[-1].name + " '" + variables_to_export[-1].default_value + "'")
                        
                        #This is where it searches for the next variable
                
                elif variables_to_export[-1].data_type != None:
                    variables_to_export[-1].name = word
                    _log.append("------------------------------------------------")
                    _log.append("Found name for variable: "+ word)

                elif word in ['int', 'std::string', 'float', 'bool']:
                    if word == '': continue
                    _log.append("------------------------------------------------")
                    _log.append("Found variable under attribute list "+ word)
                    variables_to_export[-1].attributes = currently_evaluated_attributes
                    variables_to_export[-1].data_type = word
                elif word not in ['int', 'std::string', 'float', 'bool']:
                    if word == '': continue
                    if word == 'class':
                        
                        variables_to_export[-1].attributes = currently_evaluated_attributes
                        variables_to_export[-1].data_type = word
                        _log.append("[!] UPM Header Tool: Found class "+word+ " at line "+ str(line_count))
                    else:
                        _log.append("[x] UPM Header Tool: Error, Unsupported datatype '"+word+"' at line "+ str(line_count)+ " character = " + character)
                        return []


        if character in [' ', '\n', '=', ';']:
            word = ""
        else:
            word += character
    
    return variables_to_export