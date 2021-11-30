//#define _USE_MATH_DEFINES
// should appear before #include <cmath>
#define M_PI    3.14159265358979323846  /* pi */
#define INF     10000.0 // infinite
// rounding number to d decimal places
// see: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
#define roundz(x, d)    ((std::floor(((x)*std::pow(10.0, d))+0.5))/std::pow(10.0, d))
//
// area.h
//
#ifndef area_h
#define area_h
//
#include <cmath>		// for _USE_MATH_DEFINES
#include <vector>
#include "Vector2D.h"
#include "wall.h"
#include "crossing.h"
//
enum area_type{
    room,
    corridor,
    outside
};
//
class Area{
    private:
        int         area_id;
        //std::string area_type;
        //
        std::vector <Vector2D>  Vertexes;
        std::vector <Wall*>     Walls;
        std::vector <Crossing*> Crossings;

    public:
        Area(int, std::string);
        ~Area();
        //
        void    set_area_id(int _area_id)   { area_id = _area_id; }
        void    add_vertex(Vector2D);
        void    add_vertex(double, double);
        void    add_wall_auto();    // update wall based on previously provided vertex information
        void    add_wall(double, double, double, double);
        void    add_crossing(int, double, double, double, double);
        void    add_crossing(int, double, double, double, double, int, int);
        //
        bool        is_inside_room(Vector2D);
        bool        check_crossing_visibility(int, Vector2D, double);
        Vector2D    get_nearest_point(int, Vector2D);
        double      get_distance(int, Vector2D);
        //
        void    print_vertexes();   // print included vertexes on the screen
        void    print_walls();      // print included walls on the screen
        void    print_crossings();  // print included crossings on the screen
        //
        int     get_area_id()       const { return area_id; }
        int     get_n_vertexes()    const { return Vertexes.size(); }
        int     get_n_walls()       const { return Walls.size(); }
        int     get_n_crossings()   const { return Crossings.size(); }
        //
        // area_type   get_area_type()             const { return current; }
        Vector2D    get_crossing_p1(int index)  const { return Crossings[index]->get_crossing_p1(); }
        Vector2D    get_crossing_p2(int index)  const { return Crossings[index]->get_crossing_p2(); }
};
#endif
