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
#include "wall.h"
//
//Wall::Wall(int _room_id, std::string _wall_type, Vector2D _p1, Vector2D _p2){
Wall::Wall(int _room_id, Vector2D _p1, Vector2D _p2){
    room_id = _room_id;
    p1 = _p1;
    p2 = _p2;
    //wall_type = _wall_type;
}

Wall::~Wall(){
}

Vector2D Wall::get_nearest_point(Vector2D _point){
    // returns the nearest point to the given point _point
    Vector2D relative_end_point = p2 - p1;
    Vector2D relative_point = _point - p1;
    double lambda = relative_end_point.dot(relative_point);
    lambda /= relative_end_point.length2();
    //
    if (lambda <= 0.0){
        return p1;
    } else if (lambda >= 1.0){
        return p2;
    } else{
        return p1+relative_end_point*lambda;
    }

}

double Wall::get_distance(Vector2D _point){
    // (perpendicular) distance between _point and the nearest point of the wall
    Vector2D nearest_point = get_nearest_point(_point);
    double distance = (_point-nearest_point).length();
    return distance;
}
