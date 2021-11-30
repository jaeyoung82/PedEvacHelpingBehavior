 //#define _USE_MATH_DEFINES
// should appear before #include <cmath>
#define M_PI            3.14159265358979323846  /* pi */
// rounding number to d decimal places
// see: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
#define roundz(x, d)    ((std::floor(((x)*std::pow(10.0, d))+0.5))/std::pow(10.0, d))
//
//

//
// scenario.h
//
#ifndef scenario_h
#define scenario_h
//
#include <algorithm>    // std::shuffle, std::find
#include <cmath>		// for _USE_MATH_DEFINES
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <stdio.h>
#include <string>       // std::string
#include <sstream>      // std::stringstream
#include <fstream>      // std::ofstream
#include <random>		// for random number generators
#include <time.h>       //
#include <vector>
//
#include "area.h"
#include "vector2D.h"
#include "pedestrian.h"
#include "InitialVolunteer.h"
//

class Scenario {
	private:
        //
        // setup simulation parameters
        int     run_id;
        double  epsilon;
        double  scenario_period;
        double  dt;
        int     fps;
        int     n_steps;
        int     step_current;
        double  t_current;
        int     step_update;            // update frequency of volunteer's dilemma game
        double  t_no_flow;
        int     n_ped_max;
        //
        // geometry
        double  cell_size;
        double  r_ped;
        double  bottleneck_width;
        double  room_length;
        double  room_width;
        double  corridor_length;
        double  corridor_width;
        //
        // control parameters for numerical experiments
        int     n_ped;
        int     n_injured_ini;
        double  ratio_P2;               // percentage of P2 patients who need 2 volunteers
        int     n_P2;                   // the number of injured persons in P2 category
        double  d_volunteer;            // volunteer game area parameter
        double  v_reduce_factor;        // speed reduction factor
        double  v_desired_passerby_avg; // desired speed of passerby
        double  v_desired_injured_avg;  // desired speed of injured person
        double  v_max_passerby;         // maximum speed of passerby
        double  v_max_injured;          // maximum speed of injured person
        double  payoff_T;
        double  payoff_S;
        double  beta;
        double  value_d;
        double  sensory_range;
        int n_committed;
        //
        // initialize simulation
        std::vector <Area*>         Areas;
        std::vector <Pedestrian*>   Pedestrians;
        std::vector <int>           injured_id_set;
        //
        // volunteer game model
        InitialVolunteer* IniVol;
        //
        int n_need_help;
        int n_volunteers;
        int n_volunteers_highest;       // the highest value of the number of volunteers in the simulation
        int n_injured;
        //
        int freq_update1;   //
        int freq_update2;   //
        //
        int             get_ped_index(int);
        inline int      get_ped_id(int ped_index)   { return Pedestrians[ped_index]->get_id(); }
        /*
        inline double   get_min(double a, double b) { return (a>b)? b:a; }
        inline double   get_max(double a, double b) { return (a<b)? b:a; }
        inline int      get_min(int a, int b)       { return (a>b)? b:a; }
        inline int      get_max(int a, int b)       { return (a<b)? b:a; }
        */

	public:
		Scenario(int, int, int, int, double, double, double, double);
		~Scenario();
		//
		void initialize_Ped();
		void initialize_Boundary();
		void initialize_Committed();
		void add_boundary(int, std::string, double, double, double, double);
		void add_crossing(int, double, double, double, double, int, int);
		void removePed();
		//void playVolunteerGame();
		void runScenario();
		void writeTrajectory(std::ofstream&);
		void writeNvolunteers(std::ofstream&, int);
		void writeNpassersby(std::ofstream&, int);
		void writePayoff(std::ofstream&, std::vector<double>);
		std::vector<double> Get_avg_sd(std::vector<double>);
		std::vector<double> get_statistics(int type, std::vector <double> input);
		//double UniformRnd(int);
		//
		//double get_distance_B();
		//
};

#endif // SCENARIO_H_INCLUDED
