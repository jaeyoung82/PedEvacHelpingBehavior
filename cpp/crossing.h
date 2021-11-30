//
// crossing.h
//
#ifndef crossing_h
#define crossing_h
//
#include <cmath>		// for _USE_MATH_DEFINES
#include <vector>
#include "Vector2D.h"
//
class Crossing{
    private:
        int         crossing_id;
        Vector2D    p1;
        Vector2D    p2;
        //int         area1_id;
        //int         area2_id;
        double      width;
        //
        double      agent_radius;
        Vector2D    p1_effective;
        Vector2D    p2_effective;
        double      width_effective;

    public:
        //Crossing(int, Vector2D, Vector2D, int, int);
        Crossing(int, Vector2D, Vector2D);
        ~Crossing();
        void        set_crossing_id(int _crossing_id)               { crossing_id = _crossing_id; }
		void        set_crossing_point1(double p1_x, double p1_y)   { p1.x = p1_x; p1.y = p1_y; }
		void        set_crossing_point2(double p2_x, double p2_y)   { p2.x = p2_x; p2.y = p2_y; }
        void        set_crossing_points(double p1_x, double p1_y, double p2_x, double p2_y) { p1.x = p1_x; p1.y = p1_y;  p2.x = p2_x; p2.y = p2_y; }
        //
        int         get_crossing_id()   const { return crossing_id; }
		Vector2D    get_crossing_p1()   const { return p1; }
		Vector2D    get_crossing_p2()   const { return p2; }
		//
		bool        check_crossing_visibility(Vector2D, double);

};
#endif
