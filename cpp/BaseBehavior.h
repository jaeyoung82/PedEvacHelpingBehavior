#ifndef BaseBehavior_h
#define BaseBehavior_h

#include "vector2D.h"

enum class behavior_modes{
    //
    // passerby
    egressing_passerby,     // returns 0
    //
    // injured person
    recruiting_injured,     // returns 1
    waiting_injured,        // returns 2
    egressing_injured,      // returns 3
    //
    // volunteer
    approaching_volunteer,  // returns 4
    egressing_volunteer     // returns 5
};

class Pedestrian;    // Forward declaration

class BaseBehavior
{
    public:
        BaseBehavior();
        virtual ~BaseBehavior();

        // makes this virtual if extra functionality needs to be defined for behaviors.
        virtual void set_owner(Pedestrian* new_agent);

        // makes this class an abstract class
        virtual void update() = 0;

        // set functions
        void set_behavior_mode(behavior_modes new_behavior)  {my_behavior = new_behavior;}

        // get functions
        behavior_modes get_behavior_mode()  const {return my_behavior;}
        //int print_behavior_mode();

    //private:
    protected:
        Pedestrian* Ped;
        behavior_modes my_behavior;
};

#endif
