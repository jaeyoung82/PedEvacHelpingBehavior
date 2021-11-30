//
// agent.cpp
//
#include "area.h"
//
Area::Area(int _area_id, std::string _area_type){
    area_id = _area_id;
    // area_type = _area_type;
}

Area::~Area(){
}

void Area::add_vertex(Vector2D vertex_i){
    Vertexes.push_back(vertex_i);
}

void Area::add_vertex(double x, double y){
    Vertexes.push_back(Vector2D(x, y));
}

void Area::add_wall_auto(){
    // update walls based on previously provided vertex information
    // add_vertex function should be executed before run this function!
    int n_vertices = Vertexes.size();
    for (int i = 0; i < (n_vertices-1); i++){
        Wall * Wall_i = new Wall(area_id, Vertexes[i], Vertexes[i+1]);
        Walls.push_back(Wall_i);
    }
}

void Area::add_wall(double p1_x, double p1_y, double p2_x, double p2_y){
    Wall * Wall_i = new Wall(area_id, Vector2D(p1_x, p1_y), Vector2D(p2_x, p2_y));
    Walls.push_back(Wall_i);
}

void Area::add_crossing(int _crossing_id, double p1_x, double p1_y, double p2_x, double p2_y){
    Crossing * Crossing_i = new Crossing(_crossing_id, Vector2D(p1_x, p1_y), Vector2D(p2_x, p2_y));
    Crossings.push_back(Crossing_i);
}

bool Area::is_inside_room(Vector2D _point){
    // see
    // https://en.wikipedia.org/wiki/Point_in_polygon
    // https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
    // https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
    // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
    // http://www.eecs.umich.edu/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html
    //
    int n_vertices = Vertexes.size();
    // there must be at least 3 vertexes in polygon
    // if the point is outside of the extreme of existing vertexes, the point is not in the polygon
    if (n_vertices < 3)  return false;
    double min_x = Vertexes[0].x;
    double max_x = Vertexes[0].x;
    double min_y = Vertexes[0].y;
    double max_y = Vertexes[0].y;
    for (int i = 1; i < n_vertices; i++){
        Vector2D q = Vertexes[i];
        min_x = std::min(q.x, min_x);
        max_x = std::max(q.x, max_x);
        min_y = std::min(q.y, min_y);
        max_y = std::max(q.y, max_y);
    }
    if (_point.x < min_x || _point.x > max_x ||_point.y < min_y || _point.y > max_y){
        return false;
    }
    // see www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    bool inside = false;
    for (int i = 0, j = n_vertices-1; i < n_vertices; j = i++){
        if ((Vertexes[i].y >= _point.y )!=(Vertexes[j].y >= _point.y )){
            if (_point.x <= (Vertexes[j].x-Vertexes[i].x )*(_point.y-Vertexes[i].y)/(Vertexes[j].y-Vertexes[i].y)+Vertexes[i].x){
                inside = !inside;
            }
        }
    }
    return inside;
}

bool Area::check_crossing_visibility(int _crossing_id, Vector2D _point, double _agent_radius){
    Vector2D p1 = Crossings[_crossing_id]->get_crossing_p1();
    Vector2D p2 = Crossings[_crossing_id]->get_crossing_p2();
    // check whether the agent can see the crossing in front of him/her
    Vector2D p1_effective = p1+(p2-p1)*_agent_radius;
    Vector2D p2_effective = p2+(p1-p2)*_agent_radius;
    //double width_effective = (p1_effective-p2_effective).length();
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

Vector2D Area::get_nearest_point(int _wall_id, Vector2D _point){
    Vector2D p1 = Walls[_wall_id]->get_wall_p1();
    Vector2D p2 = Walls[_wall_id]->get_wall_p2();
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

double Area::get_distance(int _wall_id, Vector2D _point){
    // (perpendicular) distance between _point and the nearest point of the wall
    Vector2D nearest_point = get_nearest_point(_wall_id, _point);
    double distance = (_point-nearest_point).length();
    return distance;
}

//
//
//
void Area::print_vertexes(){
    int n_vertices = Vertexes.size();
    for (int i = 0; i < n_vertices; i++){
        std::cout << "area_" << area_id <<"\tvertex_"<< i <<"\t"<< Vertexes[i].x <<"\t"<< Vertexes[i].y <<"\n";
    }
    std::cout << "\n";
}

void Area::print_walls(){
    int n_walls = Walls.size();
    for (int i = 0; i < n_walls; i++){
        double p1_x = Walls[i]->get_wall_p1().x;
        double p1_y = Walls[i]->get_wall_p1().y;
        double p2_x = Walls[i]->get_wall_p2().x;
        double p2_y = Walls[i]->get_wall_p2().y;
        std::cout << "area_" << area_id <<"\twall_"<< i <<"\t"<< p1_x <<"\t"<< p1_y <<"\t"<< p2_x <<"\t"<< p2_y <<"\n";
    }
    std::cout << "\n";
}

void Area::print_crossings(){
    int n_crossings = Crossings.size();
    for (int i = 0; i < n_crossings; i++){
        double p1_x = Crossings[i]->get_crossing_p1().x;
        double p1_y = Crossings[i]->get_crossing_p1().y;
        double p2_x = Crossings[i]->get_crossing_p2().x;
        double p2_y = Crossings[i]->get_crossing_p2().y;
        std::cout << "area_" << area_id <<"\tcrossing_"<< i <<"\t"<< p1_x <<"\t"<< p1_y <<"\t"<< p2_x <<"\t"<< p2_y <<"\n";
    }
}

/*
bool Area::is_inside_room(Vector2D _point){
    // see
    // https://en.wikipedia.org/wiki/Point_in_polygon
    // https://www.geeksforgeeks.org/how-to-check-if-a-given-point-lies-inside-a-polygon/
    // https://stackoverflow.com/questions/217578/how-can-i-determine-whether-a-2d-point-is-within-a-polygon/2922778#2922778
    // https://stackoverflow.com/questions/11716268/point-in-polygon-algorithm
    // http://www.eecs.umich.edu/courses/eecs380/HANDOUTS/PROJ2/InsidePoly.html
    //
    int n_vertices = Vertexes.size();
    // there must be at least 3 vertexes in polygon
    // if the point is outside of the extreme of existing vertexes, the point is not in the polygon
    if (n_vertices < 3)  return false;
    double min_x = Vertexes[0].x;
    double max_x = Vertexes[0].x;
    double min_y = Vertexes[0].y;
    double max_y = Vertexes[0].y;
    for (int i = 1; i < n_vertices; i++){
        Vector2D q = Vertexes[i];
        min_x = std::min(q.x, min_x);
        max_x = std::max(q.x, max_x);
        min_y = std::min(q.y, min_y);
        max_y = std::max(q.y, max_y);
    }
    if (_point.x < min_x || _point.x > max_x ||_point.y < min_y || _point.y > max_y){
        return false;
    }
    // see www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
    bool inside = false;
    for (int i = 0, j = n_vertices-1; i < n_vertices; j = i++){
        if ((Vertexes[i].y > _point.y )!=(Vertexes[j].y > _point.y )){
            if (_point.x < (Vertexes[j].x-Vertexes[i].x )*(_point.y-Vertexes[i].y)/(Vertexes[j].y-Vertexes[i].y)+Vertexes[i].x){
                inside = !inside;
            }
        }
    }
    return inside;
}
*/
