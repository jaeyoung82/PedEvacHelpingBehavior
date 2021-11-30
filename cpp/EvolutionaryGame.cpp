#include "EvolutionaryGame.h"
#include "pedestrian.h"
#include "scenario.h"

EvolutionaryGame::EvolutionaryGame(Pedestrian* Ped_input, double value_R, double value_P, double value_T, double value_S, double beta):
    Ped_i(Ped_input), payoff_R(value_R), payoff_P(value_P), payoff_T(value_T), payoff_S(value_S), selection_pressure(beta)
{
    //
    rnd_ini = 1;    // positive number
}

EvolutionaryGame::~EvolutionaryGame(){

}

void EvolutionaryGame::update_peers(std::vector <Pedestrian*> Pedestrians_peers){
    this->Peers.clear();
    for (unsigned int j = 0; j < Pedestrians_peers.size(); j++){
        this->Peers.push_back(Pedestrians_peers[j]);
        //std::cout <<"Ped_"<< Ped_i->get_id() << " just found a passerby Ped_" << Pedestrians_passerby[j]->get_id() <<"\n";
    }
}

void EvolutionaryGame::update_injuries(std::vector <Pedestrian*> Pedestrians_injuries){
    /*
    this->Injuries.clear();
    for (unsigned int j = 0; j < Pedestrians_injuries.size(); j++){
        this->Injuries.push_back(Pedestrians_injuries[j]);
        //std::cout <<"Ped_"<< Ped_i->get_id() << " just found an injuried  Ped_" << Pedestrians_injuries[j]->get_id() <<"\n";
    }
    */
    Injuries = Pedestrians_injuries;
}

void EvolutionaryGame::update_target(Pedestrian* Pedestrian_target){
    Ped_target = Pedestrian_target;
    n_req_volunteers = Ped_target->get_n_required_volunteers();
    //if (Ped_i->get_agent_type() == agent_type::volunteer){
    if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
        // if the pedestrian is a volunteer
        n_current_volunteers = Ped_target->get_n_current_volunteers();
        // check the type of agent
        //std::cout << Ped_i->get_id() <<"\t"<< Ped_i->get_agent_type() <<"\t";
        // check the number of volunteers helping the injured person
        //std::cout << Ped_target->get_id() <<"\t"<< n_req_volunteers <<"\t"<< n_current_volunteers <<"\n";
        //
    }
}

void EvolutionaryGame::play_EvolutionaryGame(){
    //
    // check agent type
    //
    bool this_passerby = false;
    bool this_volunteer = false;
    bool this_injured = false;
    if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::passerby){
        this_passerby = true;
    }
    if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
        this_volunteer = true;
    }
    if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::injured){
        this_injured = true;
    }
    //
    // shuffle the list of neighbors before play Evolutionary game
    //
    int seed = (int)(Ped_i->UniformRnd(Ped_i->get_id())*1234567);
    std::mt19937 rnd_0(seed);
	std::shuffle(Peers.begin(), Peers.end(), rnd_0);  // shuffle numbers between 0 and n_ped_max
	int n_peers = (int)Peers.size();
	rnd_ini += 1;
	//
    // select a random neighbor to play Evolutionary game
    //
    int index_select = 0;
    if (n_peers > 0){
        int neighbor_id = Peers[index_select]->get_id(); // update the neighbor's id
        //
        // check the neighbor's agent type
        //
        bool neighbor_passerby = false;
        bool neighbor_volunteer = false;
        bool neighbor_injured = false;
        if (Peers[index_select]->Ped_AgentType()->get_agent_type() == agent_types::passerby){
            neighbor_passerby = true;
        }
        if (Peers[index_select]->Ped_AgentType()->get_agent_type() == agent_types::volunteer){
            neighbor_volunteer = true;
        }
        if (Peers[index_select]->Ped_AgentType()->get_agent_type() == agent_types::injured){
            neighbor_injured = true;
        }
        //
        // if this pedestrian is a passerby and the neighbor is a volunteer
        //
        if ((this_passerby == true)&&(neighbor_volunteer == true)){
            //
            // check the number of volunteers helping the injured person
            //
            int injuried_id = Peers[index_select]->get_target_injured_id();
            bool enough_volunteers = true;
            if (Injuries[injuried_id]->get_n_current_volunteers() < Injuries[injuried_id]->get_n_required_volunteers()){
                enough_volunteers = false;
            }
            if (enough_volunteers == false){
                //
                // check the difference of payoff
                //
                double payoff_i = Ped_i->get_payoff_now();
                double payoff_j = Peers[index_select]->get_payoff_now();
                double u_ij = payoff_i-payoff_j;
                double prob = 1.0/(1.0+std::exp(u_ij*selection_pressure));
                double rand_value = Ped_i->UniformRnd((rnd_ini+Ped_i->get_id())*1234567);
                if (rand_value <= prob){
                    //
                    // this pedestrian becomes a volunteer
                    //
                    Ped_i->reset_agent_type();
                    Ped_i->Ped_AgentType()->set_agent_type(agent_types::volunteer);
                    Ped_i->set_behavior(new Approaching_volunteer); // <-- FIX HERE!
                    Ped_i->set_helping_someone(true);
                    Ped_i->set_target_injured_id(injuried_id);
                    Ped_i->set_injured_position(Injuries[injuried_id]->get_position());
                    Ped_i->set_step_waiting(0);
                    //
                    // update the injured person
                    //
                    Injuries[injuried_id]->add_volunteer(Ped_i);
                    Injuries[injuried_id]->add_n_current_volunteers();
                }
            }
        }
        //
        // if this pedestrian is a volunteer and the neighbor is a passerby
        //
        if ((this_volunteer == true)&&(neighbor_passerby == true)){
            //
            // check the number of volunteers helping the injured person
            //
            int injuried_id = Ped_i->get_target_injured_id();
            int n_helpers_now = Injuries[injuried_id]->get_n_current_volunteers();
            int n_helpers_req = Injuries[injuried_id]->get_n_required_volunteers();
            bool enough_volunteer = true;
            if (n_helpers_now < n_helpers_req){
                enough_volunteer = false;
            }
            if (enough_volunteer == false){
                //
                // check the difference of payoff
                //
                double payoff_i = Ped_i->get_payoff_now();
                double payoff_j = Peers[index_select]->get_payoff_now();
                double u_ij = payoff_i-payoff_j;
                double prob = 1.0/(1.0+std::exp(u_ij*selection_pressure));
                double rand_value = Ped_i->UniformRnd((rnd_ini+Ped_i->get_id())*1234567);
                if (rand_value <= prob){
                    //
                    // this pedestrian becomes a passerby
                    //
                    Ped_i->reset_agent_type();
                    Ped_i->Ped_AgentType()->set_agent_type(agent_types::passerby);
                    Ped_i->set_behavior(new Egressing_passerby);
                    Ped_i->set_helping_someone(false);
                    //
                    // update the injured person
                    //
                    Injuries[injuried_id]->minus_n_current_volunteers();
                    Injuries[injuried_id]->remove_volunteer(Ped_i);
                }
            }
        }
        //
        //
        //
    }
}
