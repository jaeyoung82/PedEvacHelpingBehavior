#ifndef SocialForceModel_h
#define SocialForceModel_h
//
#include <cmath>			// for _USE_MATH_DEFINES
#include <random>			// for random number generators
#include <algorithm>		// for checking an element in a vector
#include <fstream>          // std::ofstream
#include <iomanip>          // std::setprecision
#include <vector>
//
#include "vector2D.h"
//
// forward declaration
class Pedestrian;
class Area;
//
class SocialForceModel{
	private:
	    Pedestrian* Ped_i;
	    std::vector <Area*> Areas;
	    //Area* Area_i;
	    //
	    // SFM parameters
	    //
	    int     ped_id;
        double  delta_t;
        double  r_ped;
        double  d_members;
        double  speed_max;
        double  epsilon;
        //
        // driving force
        double  speed_desired;
        double  vd;
        double  vd_group;
        double  tau;
        double  T_a;
        //
        // repulsion-interpersonal
        std::vector <Pedestrian*> Neighbors;        //
        double  C_p;
        double  l_p;
        double  d_cutoff_P;
        double  lambda_ij;
        double  k_n;
        double  k_t;
        double  reduce_factor_Cp;
        double  reduce_factor_lp;
        //
        // attraction interaction
        double  C_r0;
        double  r_0;
        //
        // repulsion-boundary
        double  C_b;
        double  l_b;
        double  d_cutoff_B;
        //
        // random number generator
        int rnd_ini;
        int step_current;
        //
        // SFM vectors
        Vector2D    e_i;
        Vector2D    driving;
        Vector2D    repulsion_B;
        Vector2D    repulsion_P;
        Vector2D    attraction;
        Vector2D    acceleration;
        Vector2D    velocity;
        Vector2D    position;
        //
        // boundary condition
        //std::vector <Area*> Areas;
        //
        // others
        /*
        inline double   get_min(double a, double b) { return (a>b)? b:a; }
        inline double   get_max(double a, double b) { return (a<b)? b:a; }
        inline int      get_min(int a, int b)       { return (a>b)? b:a; }
        inline int      get_max(int a, int b)       { return (a<b)? b:a; }
        */

	public:
		SocialForceModel(Pedestrian* Ped_input, std::vector <Area*> Areas_input);   // simple constructor
		virtual ~SocialForceModel();
		//
		void update_SFM();
        void update_ei();
        void update_driving();
        void update_driving(Vector2D);
        void update_approaching(Vector2D);
        void update_driving_group(Vector2D);
        void update_neighbors(std::vector <Pedestrian*> Pedestrians_neighbor);
        void update_CS();
        void update_CS_group();
        void update_repulsionB();
        void update_repulsionB(Area* Area_i);
		void update_attraction(Vector2D);
		void update_acceleration();
        void update_velocity();
        void update_position();
        //
        // random number generator
        double GaussRnd(double, double);
		//
        // 'set' functions
        void set_ei(Vector2D ei_now)                    { e_i = ei_now; }
        void set_speed_desired(double value)            { speed_desired = value; }
        void set_speed_max(double value)                { speed_max = value; }
        void set_acceleration(Vector2D acceleration_i)  { acceleration = acceleration_i; }
        void add_acceleration(Vector2D acceleration_i)  { acceleration += acceleration_i; }
        void set_velocity(Vector2D velocity_i)          { velocity = velocity_i; }
        void set_position(Vector2D position_i)          { position = position_i; }
        //
        // 'get' functions
        Vector2D get_ei()               const { return e_i; }
        double get_speed_desired()      const { return speed_desired; }
        double get_speed_max()          const { return speed_max; }
        Vector2D get_acceleration()     const { return acceleration; }
        Vector2D get_velocity()         const { return velocity; }
        Vector2D get_position()         const { return position; }
        //
        double get_neighbor_distance()  const { return d_cutoff_P; }
        //
}; // end of class SocialForceModel
#endif
