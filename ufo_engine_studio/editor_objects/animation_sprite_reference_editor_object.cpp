#include "animation_sprite_reference_editor_object.h"
#include "../program_state.h"
#include "../property.h"
#include "../../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{

AnimationSpriteReferenceEditorObject::AnimationSpriteReferenceEditorObject(){
    constructor_properties.clear();
    
    key = AddConstructorProperty<PropertyAssetString>("key", "");
    local_position_handle = AddConstructorProperty<PropertyVector2f>("local position", 0.0f, 0.0f);
    offset = AddConstructorProperty<PropertyVector2f>("offset", 0.0f, 0.0f);
    frame_size = AddConstructorProperty<PropertyVector2f>("frame size", 0.0f, 0.0f);
    scale = AddConstructorProperty<PropertyVector2f>("scale", 1.0f, 1.0f);
    rotation = AddConstructorProperty<PropertyFloat>("rotation", 0.0f);
    frame_index = AddConstructorProperty<PropertyFloat>("frame index", 0);
    pace = AddConstructorProperty<PropertyFloat>("pace", 0);
}

void AnimationSpriteReferenceEditorObject::OnSetupProperty(PropertyFloat *_p){
    ActorEditorObject::OnSetupProperty(_p);

    if(_p->name == "rotation"){
        rotation = _p;
    }
    if(_p->name == "frame index"){
        frame_index = _p;
    }
    if(_p->name == "pace"){
        pace = _p;
    }

}

void AnimationSpriteReferenceEditorObject::OnSetupProperty(PropertyAssetString* _p){

    ActorEditorObject::OnSetupProperty(_p);

    if(_p->name == "key"){
        key = _p;
    }
}

void AnimationSpriteReferenceEditorObject::OnSetupProperty(PropertyString* _p){

    ActorEditorObject::OnSetupProperty(_p);

    
}

void AnimationSpriteReferenceEditorObject::OnSetupProperty(PropertyVector2f* _p){
    ActorEditorObject::OnSetupProperty(_p);

    if(_p->name == "offset"){
        offset = _p;
    }
    if(_p->name == "scale"){
        scale = _p;
    }
    if(_p->name == "frame size"){
        frame_size = _p;
    }
}

void AnimationSpriteReferenceEditorObject::OnSetupProperty(PropertyInt* _p){
    ActorEditorObject::OnSetupProperty(_p);
}

void AnimationSpriteReferenceEditorObject::Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position){
    //ActorEditorObject::Update(_program_state, _parent_editor_object, _parent_position);

    //rotation->value += 0.050f;

    Vector2f global_position = _parent_position+local_position_handle->AsVector2f();

    ImVec2 im_window_position_plus_global_position = ImAdd(ImGui::GetWindowPos(), ImVec2(global_position.x, global_position.y));

    if(!_program_state->asset_manager.textures.count(key->asset_path)){
        for(const auto& [k,asset] : _program_state->asset_manager.textures){
            if(asset->alias == "?") key->asset_path = k;
        }
    }

    //Get the texture wrapper wth the alias and the texture
    TextureWrapper* tex = _program_state->asset_manager.textures.at(key->asset_path).get();

    //Get the draw list to add to
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    float angle = rotation->value;
    Vector2f v_offset = offset->AsVector2f();
    Vector2f v_scale = scale->AsVector2f();
    
    //An image is not supposed to be of dimension 0
    if(frame_size->x == 0.0f) frame_size->x = tex->texture->w;
    if(frame_size->y == 0.0f) frame_size->y = tex->texture->h;

    //Calculate how many frames there are horizontally
    int frames_per_row = tex->texture->w / int(frame_size->x);
    //Calculate how many frames there are vertically
    int frames_per_column = tex->texture->h / int(frame_size->y);

    //Round the frame size up if the image can't be evenly divided into multiple smaller images
    if((int)frame_size->x % tex->texture->w != 0) frame_size->x = std::ceil((float)tex->texture->w / (float)frames_per_row);
    if((int)frame_size->y % tex->texture->h != 0) frame_size->y = std::ceil((float)tex->texture->h / (float)frames_per_column);

    //Recalculate if needed since frame_size might have changed
    frames_per_row = tex->texture->w / int(frame_size->x);
    frames_per_column = tex->texture->h / int(frame_size->y);

    //If there are no frames per row
    if(frames_per_row == 0){
        frames_per_row = 1;
        frame_size->x = tex->texture->w;
    }
    //If there are no frames per column
    if(frames_per_column == 0){
        frames_per_column = 1;
        frame_size->y = tex->texture->h;
    }
    
    frame_index->value = ufoMaths::Wrap(frame_index->value, 0.0f, (float)(frames_per_row*frames_per_column));

    int frame_row_number_y = frame_index->value/frames_per_row;
    int frame_column_number_x = std::fmod(frame_index->value,frames_per_row);

    float normalised_row_number = (float)frame_row_number_y / (float)frames_per_column;
    float normalised_height = 1.0f/frames_per_column;

    float normalised_column_number = (float)frame_column_number_x / (float)frames_per_row;
    float normalised_width = 1.0f/frames_per_row;

    //These represent the points where they're drawn on screen, given that they are not rotated
    ImVec2 p1 = ImAdd(im_window_position_plus_global_position, ImVec2(-v_offset.x * v_scale.x, -v_offset.y * v_scale.y)); //Window position
    ImVec2 p2 = ImAdd(
                    ImAdd(im_window_position_plus_global_position, ImVec2(frame_size->x*v_scale.x, 0.0f)),
                    ImVec2(-v_offset.x * v_scale.x, -v_offset.y * v_scale.y)
                );
    ImVec2 p3 = ImAdd(
                    ImAdd(im_window_position_plus_global_position, ImVec2(frame_size->x*v_scale.x, frame_size->y*v_scale.y)),
                    ImVec2(-v_offset.x * v_scale.x, -v_offset.y * v_scale.y)
                );
    ImVec2 p4 = ImAdd(
                    ImAdd(im_window_position_plus_global_position, ImVec2(0.0f, frame_size->y*v_scale.y)),
                    ImVec2(-v_offset.x * v_scale.x, -v_offset.y * v_scale.y)
                );

    /*ImVec2 p1 = im_window_position_plus_global_position; //Window position
    ImVec2 p2 = ImAdd(
                    im_window_position_plus_global_position, ImVec2(frame_size->x*v_scale.x, 0.0f)
                );
    ImVec2 p3 = ImAdd(
                    im_window_position_plus_global_position, ImVec2(frame_size->x*v_scale.x, frame_size->y*v_scale.y)
                );
    ImVec2 p4 = ImAdd(
                    im_window_position_plus_global_position, ImVec2(0.0f, frame_size->y*v_scale.y)
                );*/

    ImVec2 centre = ImVec2(im_window_position_plus_global_position.x, im_window_position_plus_global_position.y);

    //The normal points but rotated around point centre based on the variable offset->value
    float p1_initial_angle = atan2(p1.y - centre.y, p1.x - centre.x);
    float p2_initial_angle = atan2(p2.y - centre.y, p2.x - centre.x);
    float p3_initial_angle = atan2(p3.y - centre.y, p3.x - centre.x);
    float p4_initial_angle = atan2(p4.y - centre.y, p4.x - centre.x);

    //The angle and distance from point centre is how the new p1,p2,p3 and p4 are calculated
    float p1_distance = std::sqrt((p1.x - centre.x)*(p1.x - centre.x) + (p1.y - centre.y)*(p1.y - centre.y)) ;
    float p2_distance = std::sqrt((p2.x - centre.x)*(p2.x - centre.x) + (p2.y - centre.y)*(p2.y - centre.y)) ;
    float p3_distance = std::sqrt((p3.x - centre.x)*(p3.x - centre.x) + (p3.y - centre.y)*(p3.y - centre.y)) ;
    float p4_distance = std::sqrt((p4.x - centre.x)*(p4.x - centre.x) + (p4.y - centre.y)*(p4.y - centre.y)) ;

    //Handle some special cases since atan2(0,0) would not give a valid result
    if(p1.y == 0.0f && p1.x == 0.0f) p1_initial_angle = 0.0f;
    if(p2.y == 0.0f && p2.x == 0.0f) p2_initial_angle = 0.0f;
    if(p3.y == 0.0f && p3.x == 0.0f) p3_initial_angle = 0.0f;
    if(p4.y == 0.0f && p4.x == 0.0f) p4_initial_angle = 0.0f;

    //here p1_distance, p2_distance etc are utilised
    ImVec2 rotated_p1 = ImVec2(centre.x + std::cos(angle + p1_initial_angle) * p1_distance, centre.y + std::sin(angle + p1_initial_angle) * p1_distance);
    ImVec2 rotated_p2 = ImVec2(centre.x + std::cos(angle + p2_initial_angle) * p2_distance, centre.y + std::sin(angle + p2_initial_angle) * p2_distance);
    ImVec2 rotated_p3 = ImVec2(centre.x + std::cos(angle + p3_initial_angle) * p3_distance, centre.y + std::sin(angle + p3_initial_angle) * p3_distance);
    ImVec2 rotated_p4 = ImVec2(centre.x + std::cos(angle + p4_initial_angle) * p4_distance, centre.y + std::sin(angle + p4_initial_angle) * p4_distance);

    //This adds the actual draw call
    draw_list->AddImageQuad((ImTextureID)(intptr_t)(tex->texture),
        rotated_p1,
        rotated_p2,
        rotated_p3,
        rotated_p4,
        
        //These are the normalised local coorinates on the sprite-sheet calculated by the frame_index and
        // frame_size.
        ImVec2(normalised_column_number, normalised_row_number),
        ImVec2(normalised_column_number+normalised_width, normalised_row_number),
        ImVec2(normalised_column_number+normalised_width, normalised_row_number+normalised_height),
        ImVec2(normalised_column_number, normalised_row_number+normalised_height)
    
        );
}

}