#ifndef EvolutionaryGame_h
#define EvolutionaryGame_h
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
class EvolutionaryGame{
	private:
	    Pedestrian* Ped_i;
	    Pedestrian* Ped_target;
	    std::vector <Pedestrian*> Peers;
	    std::vector <Pedestrian*> Injuries;
	    //
	    //int n_required_volunteers;
	    double beta;
	    // std::vector <Area*> Areas;
        //
	    // Evolutionary Game parameters
	    //
        double payoff_R;
        double payoff_P;
        double payoff_T;
        double payoff_S;
        double selection_pressure;  // selection pressure
        int rnd_ini;
        //
        // volunteer
        int n_req_volunteers;
        int n_current_volunteers;

	public:
		EvolutionaryGame(Pedestrian* Ped_input, double, double, double, double, double);   // simple constructor
		virtual ~EvolutionaryGame();
		//
		void update_peers(std::vector <Pedestrian*> Pedestrians_peers);
		void update_injuries(std::vector <Pedestrian*> Pedestrians_injuries);
		void update_target(Pedestrian* Pedestrian_target);
		void play_EvolutionaryGame();
		//
        // 'set' functions
        //
        // 'get' functions
        //
        //
}; // end of class EvolutionaryGame
#endif
