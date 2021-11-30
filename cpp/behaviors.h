//#define _USE_MATH_DEFINES	// should appear before #include <cmath>
#define M_PI            3.14159265358979323846  /* pi */
//
// rounding number to d decimal places
// see: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
#define roundz(x, d)    ((std::floor(((x)*std::pow(10.0, d))+0.5))/std::pow(10.0, d))
#define M_epsilon       0.0001
#define get_min(a,b)    ((a>b)? b:a)
#define get_max(a,b)    ((a<b)? b:a)
//
#ifndef behaviors_h
#define behaviors_h

#include <iostream>     // std::cout, std::fixed
//
#include "BaseBehavior.h"
//

class Pedestrian;    // Forward declaration
//class SocialForceModel;

/*
// example
class Behaviors : public BaseBehavior
{
    public:
        Behaviors();
        ~Behaviors();

        virtual void update()
        //void update() override;
};
*/

//-----------------------------------------------------
// Passerby behaviors
//-----------------------------------------------------

class Egressing_passerby : public BaseBehavior {
    public:
        Egressing_passerby();
        ~Egressing_passerby();

        virtual void update();  // this must be implemented!
        void set_egress_direction();

    private:
        Vector2D target;
        Vector2D e_i;
};

//-----------------------------------------------------
// Injured person behaviors
//-----------------------------------------------------

class Recruiting_injured : public BaseBehavior {
    public:
        Recruiting_injured();
        ~Recruiting_injured();

        virtual void update();  // this must be implemented!
        //void play_volunteer_game();
        void set_moving_direction();
        void set_volunteer_positions();

    private:
        Vector2D target;
        Vector2D e_i;
        Vector2D ei_ini;
};

class Waiting_injured : public BaseBehavior {
    public:
        Waiting_injured();
        ~Waiting_injured();

        virtual void update();  // this must be implemented!
        void set_moving_direction();
        void set_volunteer_positions();

    private:
        int step_waiting;
        Vector2D target;
        Vector2D e_i;
};

class Egressing_injured : public BaseBehavior {
    public:
        Egressing_injured();
        ~Egressing_injured();

        virtual void update();  // this must be implemented!
        void set_moving_direction();

    private:
        int step_waiting;
        Vector2D target;
        Vector2D e_i;
};

//-----------------------------------------------------
// Volunteer behaviors
//-----------------------------------------------------

class Approaching_volunteer : public BaseBehavior {
    //
    // A volunteer is walking to the injured person who needs help
    //
    public:
        Approaching_volunteer();
        ~Approaching_volunteer();

        virtual void update();  // this must be implemented!
        void set_moving_direction();
        void set_volunteer_positions();

    private:
        Vector2D target;
        Vector2D e_i;
};

class Egressing_volunteer : public BaseBehavior {
    //
    // A volunteer is egressing with the injured person
    //
    public:
        Egressing_volunteer();
        ~Egressing_volunteer();

        virtual void update();  // this must be implemented!

    private:
        Vector2D target;
        Vector2D e_i;
};

#endif
