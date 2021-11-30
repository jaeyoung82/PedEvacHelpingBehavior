//
// scenario.cpp
//
#include "scenario.h"
//
//
Scenario::Scenario(int run_i, int n_peds, int n_injured, int input_committed, double value_T, double value_S, double beta_i, double value_ls){
    //
    // setup simulation parameters
    run_id = run_i;
    epsilon = 0.01*0.01;
    //
    // related to simulation time
    scenario_period = 90.0;     // seconds, 60 for 100 pedestrians
    dt = 0.05;                  // seconds, 0.05
    fps = 20;                   // frame per second
    n_steps = (int)((scenario_period+epsilon)*fps)+1;
    t_current = 0.0;
    step_update = fps;
    t_no_flow = 10.0;
    //
    freq_update1 = 4;  // update frequency of initial volunteer assignment (default 0.5 sec)
    freq_update2 = 4;  // update frequency of Evolutionary game (default 0.1 sec)
    //
    // setup geometry
    cell_size = 0.5;
    r_ped = 0.2;
    bottleneck_width = 2.0;
    room_length = 10.0;
    room_width = 10.0;
    corridor_length = 5.0;
    corridor_width = bottleneck_width;
    //
    // control parameters for research
    n_ped = n_peds;
    n_injured_ini = n_injured;
    ratio_P2 = 1.0;                 // ratio of injuries who need 2 volunteers
    n_P2 = (int)(n_injured_ini*(ratio_P2+epsilon));
    d_volunteer = 3.0;              // range of volunteer game
    v_reduce_factor = 0.5;          // speed reduction factor of injured person
    v_desired_passerby_avg = 1.2;   // desired speed of passerby
    v_desired_injured_avg = v_desired_passerby_avg*v_reduce_factor;    // desired speed of injured person
    v_max_passerby = 2.0;           // maximum speed of passerby
    v_max_injured = v_max_passerby*v_reduce_factor; // maximum speed of passerby
    //
    payoff_T = value_T; //
    payoff_S = value_S; //
    beta = beta_i;      // selection pressure
    sensory_range = value_ls;
    n_committed = input_committed;
    //
    // others
    n_volunteers = 0;
    n_volunteers_highest = 0;
}

Scenario::~Scenario(){
}

void Scenario::initialize_Boundary(){
    //
    // setup boundary condition for area 0: room
    Area * Area_0 = new Area(0, "room");
    Area_0->add_vertex(0.0, 0.5*bottleneck_width);      // vertex 0
    Area_0->add_vertex(0.0, 0.5*room_width);            // vertex 1
    Area_0->add_vertex(-room_length, 0.5*room_width);   // vertex 2
    Area_0->add_vertex(-room_length, -0.5*room_width);  // vertex 3
    Area_0->add_vertex(0.0, -0.5*room_width);           // vertex 4
    Area_0->add_vertex(0.0, -0.5*bottleneck_width);     // vertex 5
    Area_0->add_wall_auto();                            // update boundary based on the provided vertex information
    // Crossings[0] between the room and the corridor
    Area_0->add_crossing(0, 0.0, 0.5*bottleneck_width, 0.0, -0.5*bottleneck_width);
    Areas.push_back(Area_0);
    //
    // setup boundary condition for area 1: corridor
    Area * Area_1 = new Area(1, "corridor");
    Area_1->add_vertex(0.0, 0.5*bottleneck_width);               // vertex 0
    Area_1->add_vertex(corridor_length, 0.5*bottleneck_width);   // vertex 1
    Area_1->add_vertex(0.0, -0.5*bottleneck_width);              // vertex 2
    Area_1->add_vertex(corridor_length, -0.5*bottleneck_width);  // vertex 3
    Area_1->add_wall(0.0, 0.5*bottleneck_width, corridor_length, 0.5*bottleneck_width);
    Area_1->add_wall(0.0, -0.5*bottleneck_width, corridor_length, -0.5*bottleneck_width);
    // Crossings[1] between the corridor and the outside
    Area_1->add_crossing(1, corridor_length, 0.5*bottleneck_width, corridor_length, -0.5*bottleneck_width);
    Areas.push_back(Area_1);
}

void Scenario::initialize_Ped(){
    // generate initial positions in a room
    //
    // 1 initialize all points
    // control the value of gap_from_wall to keep a certain distance from boundaries
    double gap_from_wall = 0.5; // 0.5 or 1.0
    int n_x = (int)((room_length-2*gap_from_wall+epsilon)/cell_size);
    int n_y = (int)((room_width-2*gap_from_wall+epsilon)/cell_size);
    n_ped_max = n_x*n_y;
    int point_id_temp = 0;
    std::vector<Vector2D> point_ini;
    std::vector<int> point_id;  // point_id.size() == n_ped_max
    for (int i = 0; i < n_x; i++){
        double point_x = roundz(((-1.0)*(room_length-gap_from_wall)+(0.5+i)*cell_size), 2);
        for (int j = 0; j < n_y; j++){
            double point_y = roundz((-0.5)*(room_width-2*gap_from_wall)+(0.5+j)*cell_size, 2);
            point_ini.push_back(Vector2D(point_x, point_y));
            point_id.push_back(point_id_temp);
            point_id_temp += 1;
        }
    }
    //
    // 2 select n_ped points among the above generated points
    // 2a shuffle point_id
    int seed = run_id;
    std::mt19937 rnd_0(seed);
	std::shuffle(point_id.begin(), point_id.end(), rnd_0);  // shuffle numbers between 0 and n_ped_max
    //
    // 3 assign initial positions and other variables
    // 3a initialize position of injured persons
    //
    int ped_index = 0;
    injured_id_set.clear();
    int ped_id = 0;
    std::vector<Vector2D> injured_position_ini;
    while (ped_id < n_injured_ini){
        // get initial position
        double x_ini = point_ini[point_id[ped_index]].x;
        double y_ini = point_ini[point_id[ped_index]].y;
        bool add_new_ped = false;
        //
        // check potential conflicts with existing injured persons
        int n_potential_conflicts = 0;
        int injured_position_ini_size = (int)injured_position_ini.size();
        /*
        //
        // check potential conflicts with boundaries
        int n_walls = Areas[0]->get_n_walls();
        for (int k = 0; k < n_walls; k++){
            // check whether the boundary type corresponds to the area type
            Vector2D displacement = Vector2D(x_ini, y_ini) - Areas[0]->get_nearest_point(k, Vector2D(x_ini, y_ini));
            double distance = get_max(displacement.length(), epsilon);
            if (distance <= (3.0*r_ped)){
                //std::cout << "Ped_"<< ped_id <<" has conflicts with boundaries "<< x_ini <<" "<< y_ini << "\n";
                n_potential_conflicts += 1;
            }
        }
        */
        //
        // check potential conflicts with other injured persons
        // control the value of gap_group to keep a certain distance from different groups (injured+volunteers)
        double gap_group = 1.5; // min: 6.0*r_ped (= 1.2)
        if (injured_position_ini_size > 0 ){
            for (int j = 0; j < injured_position_ini_size; j++){
                Vector2D delta_ij = injured_position_ini[j]-Vector2D(x_ini, y_ini);
                if (delta_ij.length2() < gap_group*gap_group){
                    n_potential_conflicts += 1;
                }
            }
        }
        //
        //
        if (n_potential_conflicts == 0){
            // no conflict with existing injured persons
            add_new_ped = true;
        } else{
            add_new_ped = false;
        }
        //
        // update relevant information of injured persons
        if (add_new_ped == true){
            int n_required_volunteer = 2;
            //
            Pedestrian * Ped_i = new Pedestrian(ped_id, Vector2D(x_ini, y_ini), Areas, payoff_T, payoff_S, sensory_range, beta, n_required_volunteer, run_id, n_steps, n_ped); // here, i == id
            //Ped_i->set_agent_type(agent_type::injured);
            Ped_i->Ped_AgentType()->set_agent_type(agent_types::injured);
            Ped_i->initialize_behavior(new Recruiting_injured);
            //
            //std::cout << Ped_i->get_id() <<" "<< Ped_i->get_agent_type() <<"\n";
            //
            Ped_i->set_n_required_volunteers(n_required_volunteer);
            if (n_required_volunteer == 1){
                //Ped_i->Ped_rescue_points();
            }
            //
            //Ped_i->set_move_group(false);
            //Ped_i->set_group_id(-1);
            Pedestrians.push_back(Ped_i);
            //
            injured_position_ini.push_back(Vector2D(x_ini, y_ini));
            ped_id += 1;
            injured_id_set.push_back(ped_id);
        }
        //
        ped_index++;
    }
    // 3b initialize other persons (i.e., passerby) position
    for (int i = 0; i < (n_ped-n_injured_ini); i++){
        // get initial position
        double x_ini = point_ini[point_id[ped_index]].x;
        double y_ini = point_ini[point_id[ped_index]].y;
        //
        Pedestrian * Ped_i = new Pedestrian(ped_id, Vector2D(x_ini, y_ini), Areas, payoff_T, payoff_S, sensory_range, beta, 2, run_id, n_steps, n_ped); // here, i == id
        Ped_i->Ped_AgentType()->set_agent_type(agent_types::passerby);
        Ped_i->initialize_behavior(new Egressing_passerby);
        //Ped_i->set_move_group(false);
        //Ped_i->set_group_id(-1);
        Pedestrians.push_back(Ped_i);
        //
        ped_index += 1;
        ped_id += 1;
    }
	//
	std::vector<Vector2D>().swap(injured_position_ini); //  deallocate the memory taken by the vector
}

void Scenario::initialize_Committed(){
    //
    // initialize the committed volunteers
    //
    int n_committed_temp = 0;
    if (step_current == freq_update1){
        for (unsigned int i = 0; i < Pedestrians.size(); i++){
            if (Pedestrians[i]->Ped_AgentType()->print_agent_type() == 2){ // check injured persons
                if (Pedestrians[i]->get_area_id() == 0){ // in the room
                    //
                    //
                    // if the injured pedestrian is in the room, find a volunteer within the sensory range
                    if (Pedestrians[i]->get_rescued() == false){
                        //
                        Pedestrians[i]->check_neighbors(Pedestrians);
                        //
                        if (n_committed_temp < n_committed){
                            // add a committed volunteer
                            Pedestrians[i]->Ped_iniVol()->assign_IniVolunteers(true);
                            //Pedestrians[i]->set_commitment(true);
                            n_committed_temp += 1;
                        } else{
                            // add a non-committed volunteer
                            Pedestrians[i]->Ped_iniVol()->assign_IniVolunteers(false);
                        }
                        //
                        // TEST
                        //
                        //std::cout << Pedestrians[i]->get_id() <<"\t"<< n_committed_temp <<"\n";
                    }
                }
            }
        }
    }
}

void Scenario::removePed(){
	for (unsigned int i = 0; i < Pedestrians.size();){
		if (Pedestrians[i]->get_remove() == true) {
			//
			// find the pedestrian to be removed; the pedestrian does not exist in the corridor
			Pedestrians.erase(Pedestrians.begin()+i);
			// check whether the pedestrian is an injured one
			if (Pedestrians[i]->Ped_AgentType()->get_agent_type() == agent_types::injured){
                int injured_id = Pedestrians[i]->get_id();
                // erase the removed injured person from the vector by value
                // see https://stackoverflow.com/questions/3385229/c-erase-vector-element-by-value-rather-than-by-position
                injured_id_set.erase(std::remove(injured_id_set.begin(), injured_id_set.end(), injured_id), injured_id_set.end());
			}
		} else{
            ++i;
		}
	}
}

void Scenario::runScenario(){
    //
    int sign_S = 0;
    if (std::abs(payoff_S) > 0){
        if ((payoff_S/std::abs(payoff_S)) > 0){
            sign_S = 1;
        } else{
            sign_S = -1;
        }
    }
    //
    std::stringstream filename_common1;
    filename_common1<< n_ped <<"_"<< n_injured_ini <<"_"<< n_committed
                    <<"_b" << (int)((beta+epsilon)*100)
                    <<"_ls"<< (int)((sensory_range+epsilon)*10)
                    <<"_T" << (int)((payoff_T+epsilon)*100)
                    <<"_S" << (int)((payoff_S+sign_S*epsilon)*100)
                    <<"_"  << run_id <<".txt";
    std::stringstream filename_common2;
    filename_common2<< n_ped <<"_"<< n_injured_ini <<"_"<< n_committed
                    <<"_b" << (int)((beta+epsilon)*100)
                    <<"_ls"<< (int)((sensory_range+epsilon)*10)
                    <<"_T" << (int)((payoff_T+epsilon)*100)
                    <<"_S" << (int)((payoff_S+sign_S*epsilon)*100) <<".txt";
    //
    // open a file to write trajectories
	//std::stringstream filename_trajectory;
	//filename_trajectory << "trajectory_"<< filename_common1.str().c_str();
	//std::ofstream write_trajectory(filename_trajectory.str().c_str(), std::ios_base::out); // enable this for generating traj files.
    //
    // open a file to write payoff
	std::stringstream filename_payoff;
	filename_payoff << "payoff_"<< filename_common1.str().c_str();
    std::ofstream write_payoff(filename_payoff.str().c_str(), std::ios_base::out|std::ios_base::app);
    //
    //
	//
	initialize_Boundary();
	initialize_Ped();
	//
	t_current = 0.0;
	step_current = 0;
	n_need_help = 0; // temporary assign a value to n_need_help
	//
	std::vector<int> n_ped_now;
	if (n_ped <= n_ped_max){
        for (int t_step = 0; t_step < n_steps; t_step++){
            //n_not_rescued = 0;  // reset n_not_rescued for each simulation time step
            int n_volunteers_now = 0;   // initialize the number of volunteers
            int n_passersby_now = 0;    // initialize the number of passersby
            //
            std::vector<double> count_CC; // volunteer-volunteer
            std::vector<double> count_CD; // volunteer-passerby
            std::vector<double> count_DD; // passerby-passerby
            std::vector<double> count_DC; // passerby-volunteer
            std::vector<double> payoff_passersby;
            std::vector<double> payoff_volunteers;
            std::vector<double> payoff_statistics;
            //
            if (Pedestrians.size() > 0){
                t_current = t_step*dt;
                //
                removePed();
                //
                // write trajectory
                //writeTrajectory(write_trajectory);
                //
                // assign initial volunteers
                if (step_current == freq_update1){
                    initialize_Committed();
                }
                //
                // TEST
                //
                //std::cout << t_step <<"\t"<< Pedestrians.size() <<"\n";
                //
                // update movement
                for (unsigned int i = 0; i < Pedestrians.size(); i++){
                    //
                    // initialization
                    //
                    int count_CC_i = 0;
                    int count_CD_i = 0;
                    int count_DD_i = 0;
                    int count_DC_i = 0;
                    double payoff_i = 0.0;
                    //
                    // prepare
                    //
                    Pedestrians[i]->check_neighbors(Pedestrians);
                    if (step_current > freq_update1){
                        Pedestrians[i]->prep_EvolGame(Pedestrians);
                    }
                    //
                    // update
                    //
                    Pedestrians[i]->update(Pedestrians, step_current);
                    //
                    double ped_x = Pedestrians[i]->get_position().x;
                    if (ped_x <= 0.0){
                        if (Pedestrians[i]->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
                            // check volunteers
                            n_volunteers_now += 1;
                            count_CC_i = Pedestrians[i]->get_n_neighbor_CC();
                            count_CD_i = Pedestrians[i]->get_n_neighbor_CD();
                            payoff_i = Pedestrians[i]->get_payoff_now();
                            count_CC.push_back(count_CC_i);
                            count_CD.push_back(count_CD_i);
                            payoff_volunteers.push_back(payoff_i);
                        } else if (Pedestrians[i]->Ped_AgentType()->get_agent_type() == agent_types::passerby){
                            // check passersby
                            n_passersby_now += 1;
                            count_DD_i = Pedestrians[i]->get_n_neighbor_DD();
                            count_DC_i = Pedestrians[i]->get_n_neighbor_DC();
                            payoff_i = Pedestrians[i]->get_payoff_now();
                            count_DD.push_back(count_DD_i);
                            count_DC.push_back(count_DC_i);
                            payoff_passersby.push_back(payoff_i);
                        } else{
                            //
                        }
                    }
                } // end of for loop
            }
            //
            // prepare payoff_statistics
            //
            // n_passersby, statistics_payoff_D (avg, sd), statistics_count_DD (avg, sd), statistics_count_DC (avg, sd),
            // n_volunteers, statistics_payoff_C (avg, sd), statistics_count_CD (avg, sd), statistics_count_CC (avg, sd)
            //
            int stat_type = 1;
            //
            std::vector<double> statistics_payoff_D = get_statistics(stat_type, payoff_passersby);
            std::vector<double> statistics_count_DD = get_statistics(stat_type, count_DD);
            std::vector<double> statistics_count_DC = get_statistics(stat_type, count_DC);
            //
            payoff_statistics.push_back(n_passersby_now);           // index 0
            payoff_statistics.push_back(statistics_payoff_D[0]);    // index 1
            payoff_statistics.push_back(statistics_payoff_D[1]);    // index 2
            payoff_statistics.push_back(statistics_count_DD[0]);    // index 3
            payoff_statistics.push_back(statistics_count_DD[1]);    // index 4
            payoff_statistics.push_back(statistics_count_DC[0]);    // index 5
            payoff_statistics.push_back(statistics_count_DC[1]);    // index 6
            //
            std::vector<double> statistics_payoff_C = get_statistics(stat_type, payoff_volunteers);
            std::vector<double> statistics_count_CC = get_statistics(stat_type, count_CC);
            std::vector<double> statistics_count_CD = get_statistics(stat_type, count_CD);
            //
            payoff_statistics.push_back(n_volunteers_now);          // index 7
            payoff_statistics.push_back(statistics_payoff_C[0]);    // index 8
            payoff_statistics.push_back(statistics_payoff_C[1]);    // index 9
            payoff_statistics.push_back(statistics_count_CD[0]);    // index 10
            payoff_statistics.push_back(statistics_count_CD[1]);    // index 11
            payoff_statistics.push_back(statistics_count_CC[0]);    // index 12
            payoff_statistics.push_back(statistics_count_CC[1]);    // index 13

            //
            if (step_current == freq_update1){
                writePayoff(write_payoff, payoff_statistics);
            } else if ((step_current > freq_update1)&&((step_current%freq_update2)==0)){
                writePayoff(write_payoff, payoff_statistics);
            } else{
                //
            }
            //
            // update the step
            step_current += 1;
        }
	}
    write_payoff.close();
    //
    //write_trajectory.close(); // enable this for generating traj files.
    //
}

void Scenario::writeTrajectory(std::ofstream& write_trajectory){
    for (unsigned int i = 0; i < Pedestrians.size(); i++){
        // write trajectory
        write_trajectory << std::setprecision(0)<< step_current <<"\t"<< Pedestrians[i]->get_id();
        write_trajectory <<"\t"<< std::setprecision(2)<< Pedestrians[i]->get_position().x <<"\t"<< Pedestrians[i]->get_position().y;
        write_trajectory <<"\t"<< Pedestrians[i]->get_ei().x <<"\t"<< Pedestrians[i]->get_ei().y;
        write_trajectory <<"\t"<< Pedestrians[i]->get_velocity().x <<"\t"<< Pedestrians[i]->get_velocity().y <<"\t"<< Pedestrians[i]->get_velocity().length();
        write_trajectory <<"\t"<< Pedestrians[i]->Ped_AgentType()->print_agent_type();
        //write_trajectory <<"\t"<< Pedestrians[i]->get_agent_type() <<"\t"<< Pedestrians[i]->get_behavior_mode();
        write_trajectory << "\n";
        //
    }
}

void Scenario::writePayoff(std::ofstream& write_payoff, std::vector<double> payoff_statistics){
    // write information related to the number of different types of agents and their payoff
    //
    // step,
    // n_passersby, statistics_payoff_D (avg, sd), statistics_count_DD (avg, sd), statistics_count_DC (avg, sd),
    // n_volunteers, statistics_payoff_C (avg, sd), statistics_count_CD (avg, sd), statistics_count_CC (avg, sd)
    //
    write_payoff << step_current << std::setprecision(2) <<"\t";
    write_payoff << (int)(payoff_statistics[0]) <<"\t";
    write_payoff << payoff_statistics[1] <<"\t"<< payoff_statistics[2] <<"\t";
    write_payoff << payoff_statistics[3] <<"\t"<< payoff_statistics[4] <<"\t";
    write_payoff << payoff_statistics[5] <<"\t"<< payoff_statistics[6] <<"\t";
    write_payoff << (int)(payoff_statistics[7]) <<"\t";
    write_payoff << payoff_statistics[8] <<"\t"<< payoff_statistics[9] <<"\t";
    write_payoff << payoff_statistics[10] <<"\t"<< payoff_statistics[11] <<"\t";
    write_payoff << payoff_statistics[12] <<"\t"<< payoff_statistics[13];
    write_payoff << "\n";
}

//
//
int Scenario::get_ped_index(int ped_id){
    for (unsigned int j = 0; j < Pedestrians.size(); j++){
        if (ped_id == Pedestrians[j]->get_id()){
            return j;
        }
    }
    std::cout << "cannot find ped_index...\n";
    return -1;
}
//
//
//
std::vector <double> Scenario::get_statistics(int type, std::vector <double> input){
    // see also: http://stackoverflow.com/questions/7616511/calculate-mean-and-standard-deviation-from-a-vector-of-samples-in-c-using-boos
    int size_input = input.size();
    std::vector <double> results;
    // type 1: returns avg and SD
    // type 2: returns avg, SD, min and max
    if (size_input > 0){
        double sum = std::accumulate(input.begin(), input.end(), 0.0);
        double mean = sum/size_input;
        results.push_back(mean);
        //
        std::vector<double> diff(size_input);
        std::transform(input.begin(), input.end(), diff.begin(),
        std::bind2nd(std::minus<double>(), mean));
        double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / size_input);
        results.push_back(stdev);
        //
        if (type == 2){
            double value_min = *std::min_element(input.begin(), input.end());
            double value_max = *std::max_element(input.begin(), input.end());
            results.push_back(value_min);
            results.push_back(value_max);
        }
        return results;
    }
    else{
        results.push_back(0.0);
        results.push_back(0.0);
        if (type == 2){
            results.push_back(0.0);
            results.push_back(0.0);
        }
        return results;
    }
}
