#ifndef InitialVolunteer_h
#define InitialVolunteer_h
//
#include <cmath>			// for _USE_MATH_DEFINES
#include <random>			// for random number generators
#include <algorithm>		// for checking an element in a vector; std::shuffle
#include <fstream>          // std::ofstream
#include <iomanip>          // std::setprecision
#include <vector>
//
#include "vector2D.h"
//
// forward declaration
class Pedestrian;
//
class InitialVolunteer{
	private:
	    Pedestrian* Ped_i;
	    std::vector <Pedestrian*> Passersby;
	    //
	    //int n_required_volunteers;
	    double beta;
	    double p_help;
	    // std::vector <Area*> Areas;
        //
	    // Volunteer Dilemma Game parameters
	    //
        //

	public:
		InitialVolunteer(Pedestrian* Ped_input);   // simple constructor
		virtual ~InitialVolunteer();
		//
		void update_passersby(std::vector <Pedestrian*> Pedestrians_passerby);
		void assign_IniVolunteers(bool);
		//
        // 'set' functions
        //void set_n_required_volunteers(int n_required)  {n_required_volunteers = n_required;}
        //
        // 'get' functions
        //int get_n_required_volunteers()  const {return n_required_volunteers;}
        //
        //
}; // end of class InitialVolunteer
#endif
