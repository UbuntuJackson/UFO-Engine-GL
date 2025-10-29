#include "generated.h"
#include "actor.h"
#include "pony.h"
#include "ufo_macros.h"

//The 'spawn'-keyword tells the UPM Header Tool that this class is supposed to be instantiatable in the UFO-Engine.

//@attributes export_class : true;
class MyCharacter : public Actor{
public:

    //This value is not exposed and is untouched by the UPM Header Tool
    int score = 0;

    //Upon parsing, the UPM Header Tool will create a field, in the editor, where you can enter an initial hp value for
    // your MyCharacter. It's value will be limited between 0.0 and 5.0, however. The 'alias'-keyword takes a string which will appear
    // as the label of the variable in the editor.

    //@attributes display_name : "HP"; range : 0.0, 5.0; export : true;
    int hp=4;

    //Upon parsing, the UPM Header Tool will make a slider for this variable, with the minimal value being 1.0,
    // the maximum value being 500.0, and each step being 0.1

    /*
        @attributes
        slider : 1.0, 500.0, 0.1;
        export : true;
    */
    float walk_speed_magnitude = 50.0f;

    //This is an ordinary comment

    /*
        This is an ordinary multi-line comment
    */

    enum CharacterTypes{
        MONSTER,
        WIZARD,
        KNIGHT
    };

    //Upon parsing, the UPM Header Tool will create a drop-down box, each representing a potential value from the enum with identifiers
    // MOSTER, WIZARD, and NIGHT, evaluating to 0,1,2 respectively.

    /*@attributes
        alias : "Character Type";
        export_enum : "Monster", "Wizard", "Knight";
    */
    int character_type = CharacterTypes::WIZARD;

    //@attributes export : false;
    std::string name = "";

    //This variable is untouched by the UPM Header Tool.
    std::string test = "";

    //Upon parsing, the UPM Header Tool will create a text-field in the editor, where you can enter your characters nickname.

    //@attributes export : true;
    std::string nick_name = "Nick";

    // @attributes export : true;
    bool toggled = false;

    //This datatype is unsupported by the UPM Header Tool,
    // and the parser will bitch and whine about it like mad.

    /*
        attributes
        export : true;
    */
    std::unique_ptr<int> u_number;

    //An ordinary function
    void OnLevelEnter(Level* _level){
        
    }

};