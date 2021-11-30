//
// simulation.cpp
//
//#define _USE_MATH_DEFINES	// should appear before #include <cmath>
#define M_PI            3.14159265358979323846  /* pi */
//
// rounding number to d decimal places
// see: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
#define roundz(x, d)    ((std::floor(((x)*std::pow(10.0, d))+0.5))/std::pow(10.0, d))
//
#include "crossing.h"
//

//Crossing::Crossing(int _crossing_id, Vector2D _p1, Vector2D _p2, int _area1_id, int _area2_id){
Crossing::Crossing(int _crossing_id, Vector2D _p1, Vector2D _p2){
    crossing_id = _crossing_id;
    p1 = _p1;
    p2 = _p2;
    //
    width = (p1-p2).length();
}

Crossing::~Crossing(){
}

bool Crossing::check_crossing_visibility(Vector2D _point, double _agent_radius){
    // check whether the agent can see the crossing in front of him/her
    p1_effective = p1+(p2-p1)*agent_radius;
    p2_effective = p2+(p1-p2)*agent_radius;
    width_effective = (p1_effective-p2_effective).length();
    //
    Vector2D relative_end_point = p2_effective - p1_effective;
    Vector2D relative_point = _point - p1_effective;
    double lambda = relative_end_point.dot(relative_point);
    lambda /= relative_end_point.length2();
    //
    if (lambda <= 0.0){
        return false;
    } else if (lambda >= 1.0){
        return false;
    } else{
        return true;
    }
}
