#include <cstdio>
//
#include "behaviors.h"
#include "pedestrian.h"
#include "SocialForceModel.h"
#include "InitialVolunteer.h"
#include "EvolutionaryGame.h"
//
Pedestrian::Pedestrian(int this_id, Vector2D position_ini, std::vector <Area*> Areas_input,
                       double value_T, double value_S, double value_ls, double beta_i,
                       int n_required, int run_i, int _n_steps, int _n_ped)
    : current_behavior(nullptr), Areas(Areas_input), n_required_volunteers(n_required), run_id(run_i), n_steps(_n_steps), n_ped(_n_ped)
    //current_area_type(room)
    // direct initialization of member variables
{
    ped_id = this_id;
    area_id = 0;
    step_current = 0;
    freq_update1 = 4;  // update frequency of volunteer's dilemma game (default 0.5 sec)
    freq_update2 = 4;  // update frequency of Evolutionary game (default 0.1 sec)
    delta_t = 0.05;
    fps = 20;
    r_ped = 0.2;
    d_members = 0.45;
    remove_me = false;
    //
    position = position_ini;
    velocity = Vector2D(0.0, 0.0);
    acceleration = Vector2D(0.0, 0.0);
    //
    // setup social force model
    SFM = new SocialForceModel(this, Areas);
    //
    // setup initial volunteers
    IniVol = new InitialVolunteer(this);
    //
    // setup Evolutionary Game model
    payoff_R = 1.0;
    payoff_P = 0;
    payoff_T = value_T;
    payoff_S = value_S;
    play_game = false;
    payoff_now = 0.0;
    l_s = value_ls;
    beta = beta_i;
    EvolGame = new EvolutionaryGame(this, payoff_R, payoff_P, payoff_T, payoff_S, beta);
    n_game_neighbors = 0;
    n_neighbor_CC = 0;
    n_neighbor_CD = 0;
    n_neighbor_DD = 0;
    n_neighbor_DC = 0;
    //
    // initialize agent type
    Agent_type = new AgentType();
    //
    // related to injured person
    n_current_volunteers = 0;
    injured_rescued = false;
    d_iniVol = l_s; // range of initial volunteer assignment
    //
    for (int i = 0; i < 2; i ++){
        RescuePoint * RescuePoint_i = new RescuePoint(i);
        RescuePoints.push_back(RescuePoint_i);
        //delete RescuePoint_i;
    }
    //
    // related to volunteer
    helping_someone = false;
    commitment = false;
    target_injured_id = -1;
    //
    // related to passersby
    d_evolgame = l_s; // evolutionary game range parameter
}

Pedestrian::~Pedestrian(){
    // new - delete
    delete SFM;
    delete IniVol;
}

void Pedestrian::initialize_behavior(BaseBehavior* initial_behavior){
    this->set_behavior(initial_behavior);
}

void Pedestrian::reset_behavior(){
    if (current_behavior != nullptr){
        delete current_behavior; // clean up memory
        //current_behavior = nullptr;
    }
}

void Pedestrian::set_behavior(BaseBehavior* newBehavior){
    if (newBehavior != current_behavior){
        if (current_behavior != nullptr){
            //delete current_behavior; // clean up memory
        }
        current_behavior = newBehavior;
        current_behavior->set_owner(this);
    }
}

void Pedestrian::update(std::vector <Pedestrian*> Pedestrians, int step_t){
    // update current time
    step_current = step_t;

    // check which area the pedestrian is in
    this->check_area();

    // check neighboring pedestrians within a certain distance
    //this->check_neighbors(Pedestrians);

    // update behavior mode
    if (current_behavior != nullptr) {
        current_behavior->update();
    }

    //
    //std::cout << step_current <<"\t"<< ped_id <<"\n";
}

void Pedestrian::remove_from_simulation(){
    if (current_behavior != nullptr){
        // Be sure to clean up memory
        delete current_behavior;
        current_behavior = nullptr;
    }
    this->remove_me = true;
}

void Pedestrian::check_area(){
    int n_areas = (int)Areas.size();
    for (int i = 0; i < n_areas; i++){
        bool inside_room = Areas[i]->is_inside_room(this->position);
        if (inside_room == true) this->area_id = i;
    }
}

void Pedestrian::check_neighbors(std::vector <Pedestrian*> Pedestrians){
    //
    // check pedestrians within a certain distance
    //
    Neighbors.clear();      // clear the list of neighbors in social force model
    Passerby_near.clear();  // clear the list of nearby passerby
    Injuries.clear();       // clear the list for evolutionary game
    //Peer_near.clear();      // clear the list for evolutionary game
    //
    payoff_now = 0.0;
    n_game_neighbors = 0;
    n_neighbor_CC = 0;
    n_neighbor_CD = 0;
    n_neighbor_DD = 0;
    n_neighbor_DC = 0;
    play_game = false;
    //
    // check agent type
    //
    bool this_passerby = false;
    bool this_volunteer = false;
    bool this_injured = false;
    if (this->Ped_AgentType()->get_agent_type() == agent_types::passerby){
        this_passerby = true;
    }
    if (this->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
        this_volunteer = true;
    }
    if (this->Ped_AgentType()->get_agent_type() == agent_types::injured){
        this_injured = true;
    }
    //
    for (unsigned int j = 0; j < Pedestrians.size(); j++){
        int id_j = Pedestrians[j]->get_id(); // opponent's id
        //
        //
        if (id_j != ped_id){
            double d_ij_sq = (position-Pedestrians[j]->get_position()).length2();
            double d_cutoff_P = SFM->get_neighbor_distance();
            bool not_add_neighbor = false;
            //
            bool neighbor_passerby = false;
            bool neighbor_volunteer = false;
            bool neighbor_injured = false;
            if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::passerby){
                neighbor_passerby = true;
            } else if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
                neighbor_volunteer = true;
            } else if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::injured){
                neighbor_injured = true;
            } else {
                //
            }
            //
            // social force model
            //
            if (d_ij_sq < d_cutoff_P*d_cutoff_P){
                if ((this_volunteer == true)&&(this->Ped_Behavior()->get_behavior_mode() == behavior_modes::approaching_volunteer)){
                    if ((Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::injured)&&(Pedestrians[j]->get_target_injured_id() == ped_id)){
                        not_add_neighbor = true;
                        helping_someone = true;
                    }
                }
                // add to the list of neighbors
                if (not_add_neighbor == false){
                    Neighbors.push_back(Pedestrians[j]);
                }
            }
            //
            // assign initial volunteers
            //
            bool recruiting_injured = false;
            if (this->Ped_Behavior()->get_behavior_mode() == behavior_modes::recruiting_injured){
                recruiting_injured = true;
            }
            bool neighbor_helping_someone = false;
            if (Pedestrians[j]->get_helping_someone() == true){
                neighbor_helping_someone = true;
            }
            if (step_current <= freq_update1){
                if (d_ij_sq < d_iniVol*d_iniVol){
                    if ((this_injured == true)&&(recruiting_injured == true)){
                        if ((neighbor_passerby == true)&&(neighbor_helping_someone == false)){
                            // add to the list of nearby passersby
                            Passerby_near.push_back(Pedestrians[j]);
                        }
                    }
                }
            }
            //
            // check nearby injured persons
            //
            if (neighbor_injured == true){
                Injuries.push_back(Pedestrians[j]);
            }
            //
            // evaluate payoff
            //
            if (d_ij_sq < d_evolgame*d_evolgame){
                // check the strategy of this agent and the neighbor
                if ((this_volunteer == true)&&(neighbor_volunteer == true)){
                    // volunteer-volunteer interaction
                    n_game_neighbors += 1;
                    n_neighbor_CC += 1;
                    play_game = true;
                    //
                    payoff_now += payoff_R;
                    //
                    // TEST
                    if (step_current == 2*freq_update2){
                        //std::cout << step_current <<"\t"<< ped_id <<"\t"<< id_j <<"\t"<< payoff_now <<"\t+R\n";
                    }
                    //
                } else if ((this_volunteer == true)&&(neighbor_passerby == true)){
                    // volunteer-volunteer interaction
                    n_game_neighbors += 1;
                    n_neighbor_CD += 1;
                    play_game = true;
                    //
                    payoff_now += payoff_S;
                    //
                    // TEST
                    if (step_current == 2*freq_update2){
                        //std::cout << step_current <<"\t"<< ped_id <<"\t"<< id_j <<"\t"<< payoff_now <<"\t+S\n";
                    }
                    //
                } else if ((this_passerby == true)&&(neighbor_volunteer == true)){
                    // passerby-volunteer interaction
                    n_game_neighbors += 1;
                    n_neighbor_DC += 1;
                    play_game = true;
                    //
                    payoff_now += payoff_T;
                    //
                    // TEST
                    if (step_current == 2*freq_update2){
                        //std::cout << step_current <<"\t"<< ped_id <<"\t"<< id_j <<"\t"<< payoff_now <<"\t+T\n";
                    }
                    //
                } else if ((this_passerby == true)&&(neighbor_passerby == true)){
                    // passerby-passerby interaction
                    n_game_neighbors += 1;
                    n_neighbor_DD += 1;
                    play_game = true;
                    //
                    payoff_now += payoff_P;
                    //
                    // TEST
                    if (step_current == 2*freq_update2){
                        //std::cout << step_current <<"\t"<< ped_id <<"\t"<< id_j <<"\t"<< payoff_now <<"\t+P\n";
                    }
                    //
                } else{
                    //
                }
            }
            //
            // evolutionary game
            //
            /*
            if (d_ij_sq < d_evolgame*d_evolgame){
                if (this_passerby == true){
                    if (neighbor_passerby == true){
                        // add this nearby passerby to the neighbor list
                        //if (n_game_neighobors > 0)&&
                        Peer_near.push_back(Pedestrians[j]);
                    }
                    if (neighbor_volunteer == true){
                        // add this nearby volunteer to the neighbor list
                        Peer_near.push_back(Pedestrians[j]);
                    }
                }
                //
                if (this_volunteer == true){
                    if (neighbor_passerby == true){
                        // add this nearby passerby to the neighbor list
                        Peer_near.push_back(Pedestrians[j]);
                    }
                    if (neighbor_volunteer == true){
                        // add this nearby volunteer to the neighbor list
                        Peer_near.push_back(Pedestrians[j]);
                    }
                }
            }
            */
        }
    }
    //
    // update the payoff value
    if (n_game_neighbors == 0){
        play_game = false;
    } else{
        payoff_now = payoff_now/n_game_neighbors;
        //
        // TEST
        if (step_current == 2*freq_update2){
            //std::cout << step_current <<"\t"<< ped_id <<"\t"<< n_game_neighobors <<"\t"<< payoff_now <<"\n";
        }
        //
    }
    //
    // update neighbor list
    SFM->update_neighbors(Neighbors);       // Social Force Model
    IniVol->update_passersby(Passerby_near);
    //
    //EvolGame->update_peers(Peer_near);      // Evolutionary Game
    EvolGame->update_injuries(Injuries);    // Evolutionary Game
    //if (this->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
    if (this_volunteer == true){
        // if the agent is a volunteer
        EvolGame->update_target(Pedestrians[target_injured_id]);
    }
    //
    // TEST
    //
    //std::cout << step_current <<"\t"<< ped_id <<"\n";
}

void Pedestrian::prep_EvolGame(std::vector <Pedestrian*> Pedestrians){
    //
    Peer_near.clear();      // clear the list for evolutionary game
    Injuries.clear();       // clear the list for evolutionary game
    //
    // check agent type
    //
    bool this_passerby = false;
    bool this_volunteer = false;
    bool this_injured = false;
    if (this->Ped_AgentType()->get_agent_type() == agent_types::passerby){
        this_passerby = true;
    }
    if (this->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
        this_volunteer = true;
    }
    if (this->Ped_AgentType()->get_agent_type() == agent_types::injured){
        this_injured = true;
    }
    //
    // check neighbors who are playing the evolutionary game
    //
    for (unsigned int j = 0; j < Pedestrians.size(); j++){
        int neighbor_id = Pedestrians[j]->get_id(); // opponent's id
        //
        if (neighbor_id != ped_id){
            //
            double d_ij_sq = (position-Pedestrians[j]->get_position()).length2();
            //
            bool neighbor_passerby = false;
            bool neighbor_volunteer = false;
            bool neighbor_injured = false;
            if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::passerby){
                neighbor_passerby = true;
            }
            if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
                neighbor_volunteer = true;
            }
            if (Pedestrians[j]->Ped_AgentType()->get_agent_type() == agent_types::injured){
                neighbor_injured = true;
            }
            //
            // update the list of neighbors for the evolutionary game
            //
            bool this_play_EvolGame = false;
            bool neighbor_play_EvolGame = false;
            if (play_game == true){
                this_play_EvolGame = true;
            }
            if (Pedestrians[j]->get_play_game() == true){
                neighbor_play_EvolGame = true;
            }
            //
            if ((this_play_EvolGame == true)&&(neighbor_play_EvolGame == true)){
                if (d_ij_sq < d_evolgame*d_evolgame){
                    if (this_passerby == true){
                        if (neighbor_passerby == true){
                            // add this nearby passerby to the neighbor list
                            //if (n_game_neighobors > 0)&&
                            Peer_near.push_back(Pedestrians[j]);
                        }
                        else if (neighbor_volunteer == true){
                            // add this nearby volunteer to the neighbor list
                            Peer_near.push_back(Pedestrians[j]);
                        }
                    } else if (this_volunteer == true){
                        if (neighbor_passerby == true){
                            // add this nearby passerby to the neighbor list
                            Peer_near.push_back(Pedestrians[j]);
                        }
                        else if (neighbor_volunteer == true){
                            // add this nearby volunteer to the neighbor list
                            Peer_near.push_back(Pedestrians[j]);
                        }
                    } else {
                        //
                    }
                }
            }
            //
        }
    }


    //
    EvolGame->update_peers(Peer_near);      // Evolutionary Game
}

double Pedestrian::UniformRnd(int value){
	// generate random numbers
	// see http://stackoverflow.com/questions/22923551/generating-number-0-1-using-mersenne-twister-c
	int rnd_seed = (run_id*n_steps+step_current)*n_ped+value;
	//std::cout << ped_id <<"\t"<< step_current <<"\n";
	std::mt19937 rnd0(rnd_seed);									//feed random seed to use MersenneTwist random number generator
	std::uniform_real_distribution<double> dist_u(0.0, 1.0);	//generate random real number between 0 and 1
	return dist_u(rnd0);
}

double Pedestrian::GaussRnd(double _avg, double _sd, int value){
    int rnd_seed = (run_id*n_steps+step_current)*n_ped+value;
    std::mt19937 rnd0(rnd_seed);           //feed random seed to use MersenneTwist random number generator
    std::normal_distribution<> dis_Gauss(_avg, _sd);
    double _returnValue = dis_Gauss(rnd0);
    double _z = 3.0;
    if (_returnValue < _avg-_z*_sd){_returnValue = _avg-_z*_sd;}
    if (_returnValue > _avg+_z*_sd){_returnValue = _avg+_z*_sd;}
    return _returnValue;
}

void Pedestrian::remove_volunteer(Pedestrian* Volunteer){
    int id_remove = Volunteer->get_id();
    for (unsigned int i = 0; i < Volunteers.size(); i++){
        int id_temp = Volunteers[i]->get_id();
        if (id_remove == id_temp){
            // erase the i-th element
            Volunteers.erase(Volunteers.begin()+i);
            //std::cout << "remove Ped_" << id_remove <<" "<< id_temp <<"\n";
        }
    }
}

void Pedestrian::reset_agent_type(){
    if (Agent_type != nullptr){
        delete Agent_type; // clean up memory
        Agent_type = new AgentType();
    }
}

//
//
//
AgentType::AgentType(){
    //std::cout << "new agent_type\n";
}

AgentType::~AgentType(){
    //std::cout << "delete agent_type\n";
    //std::cout << "delete exiting agent type, ped_"<< ped_id <<"\n";
}

void AgentType::set_agent_type(agent_types new_type){
    my_agent_type = new_type;
    //int new_type_code = print_agent_type();
    //std::cout <<"ped_"<< ped_id <<" reset agent type (new_code = "<< new_type_code <<")\n";
}

int AgentType::print_agent_type(){
    if (my_agent_type == agent_types::passerby){
        return 0;
    }
    else if (my_agent_type == agent_types::volunteer){
        return 1;
    }
    else if (my_agent_type == agent_types::injured){
        return 2;
    }
    else{
        std::cout << "agent type error! Ped_"<< ped_id <<"\n";
        return 0;
    }
}

//
//
//
RescuePoint::RescuePoint(int id){
    rescue_point_id = id;
}

RescuePoint::~RescuePoint(){
}

//
//
//
Group::Group(int id){
    group_id = id;
    std::cout << "Ped_" << id << " created group_" << id <<"\n";
    // initialization
    member_id_0 = id;
    member_id_1 = -1;
    member_id_2 = -1;
}

Group::~Group(){

}

void Group::set_id_memebers(int id_0, int id_1){
    // set group member ids
    member_id_0 = id_0;
    member_id_1 = id_1;
    std::cout <<"group_"<< member_id_0 <<"\t"<< "volunteer # "<< member_id_1 << "n";
}

void Group::set_id_memebers(int id_0, int id_1, int id_2){
    // set group member ids
    member_id_0 = id_0;
    member_id_1 = id_1;
    member_id_2 = id_2;
    std::cout <<"group_"<< member_id_0 <<"\t"<< "volunteer # "<< member_id_1 <<"\t"<< member_id_2 <<"\n";
}
