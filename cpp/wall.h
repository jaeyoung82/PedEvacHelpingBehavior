//
// wall.h
//
#ifndef wall_h
#define wall_h
//
#include <cmath>		// for _USE_MATH_DEFINES
#include <vector>
#include "Vector2D.h"
//
class Wall {
	private:
        int         room_id;
        std::string wall_type;
        Vector2D    p1;
        Vector2D    p2;

	public:
		//Wall(int, std::string, Vector2D, Vector2D);
		Wall(int, Vector2D, Vector2D);
		~Wall();
		//
		void        set_room_id(int _room_id)               { room_id = _room_id; }
		void        set_wall_type(std::string _wall_type)   { wall_type = _wall_type; }
		void        set_wall_p1(double p1_x, double p1_y)   { p1.x = p1_x; p1.y = p1_y; }
		void        set_wall_p2(double p2_x, double p2_y)   { p2.x = p2_x; p2.y = p2_y; }
		void        set_wall_points(double p1_x, double p1_y, double p2_x, double p2_y) { p1.x = p1_x; p1.y = p1_y;  p2.x = p2_x; p2.y = p2_y; }
        //
        int         get_room_id()   const { return room_id; }
		std::string get_wall_type() const { return wall_type; }
		Vector2D    get_wall_p1()   const { return p1; }
		Vector2D    get_wall_p2()   const { return p2; }
		//
		Vector2D    get_nearest_point(Vector2D);
		double      get_distance(Vector2D);
};
#endif
