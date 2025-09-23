#include "raw_shape_base.h"
#include "../external/olcPixelGameEngine.h"
#include "rectangle.h"
#include "circle.h"
#include "pill.h"

RawShapeBase::RawShapeBase(olc::vf2d _position) : position{_position}{}

void RawShapeBase::Update(){
    //Whatever it needs to do.
}

void RawShapeBase::Draw(Camera *_camera){

}

bool RawShapeBase::IsOverlappingOther(RawShapeBase* _other){return false;}
bool RawShapeBase::IsOverlapping(Circle* _circle){return false;}
bool RawShapeBase::IsOverlapping(ufo::Rectangle* _circle){return false;}
bool RawShapeBase::IsOverlapping(Pill* _pill){return false;}