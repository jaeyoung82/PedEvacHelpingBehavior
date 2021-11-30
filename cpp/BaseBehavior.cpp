#include "pedestrian.h"
//#pragma hdrstop

#include "BaseBehavior.h"

BaseBehavior::BaseBehavior() :
    Ped(nullptr)
{
    //std::cout << "new BaseBehavior\n";
}

BaseBehavior::~BaseBehavior() {
    //std::cout << "delete BaseBehavior\n";
}

void BaseBehavior::set_owner(Pedestrian* Ped_new) {
    Ped = Ped_new;
}
