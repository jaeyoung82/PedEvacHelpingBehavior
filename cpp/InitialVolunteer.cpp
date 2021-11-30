#include "InitialVolunteer.h"
#include "pedestrian.h"
#include "scenario.h"

InitialVolunteer::InitialVolunteer(Pedestrian* Ped_input):
    Ped_i(Ped_input)
{
    //
}

InitialVolunteer::~InitialVolunteer(){
    //
}

void InitialVolunteer::update_passersby(std::vector <Pedestrian*> Pedestrians_passerby){
    this->Passersby.clear();
    for (unsigned int j = 0; j < Pedestrians_passerby.size(); j++){
        this->Passersby.push_back(Pedestrians_passerby[j]);
        //std::cout <<"Ped_"<< Ped_i->get_id() << " just found a passerby Ped_" << Pedestrians_passerby[j]->get_id() <<"\n";
    }
}

void InitialVolunteer::assign_IniVolunteers(bool commitment){
    //
    // randomly select one passerby and make him become a volunteer
    //
    // shuffle
    int seed = (int)(Ped_i->UniformRnd(Ped_i->get_id())*1234567);
    std::mt19937 rnd_0(seed);
	std::shuffle(Passersby.begin(), Passersby.end(), rnd_0);  // shuffle numbers between 0 and n_ped_max
	//
    // the injured person
    Ped_i->add_n_current_volunteers();
    int target_id = Ped_i->get_id();
    Vector2D target_position = Ped_i->get_position();
    //
    // select a volunteer
    int n_passersby = (int)Passersby.size();
    if (n_passersby > 0){
        int index_volunteer = 0;
        Passersby[index_volunteer]->reset_agent_type();
        Passersby[index_volunteer]->Ped_AgentType()->set_agent_type(agent_types::volunteer);
        Passersby[index_volunteer]->set_helping_someone(true);
        Passersby[index_volunteer]->set_initial_volunteer(true);
        Passersby[index_volunteer]->set_target_injured_id(target_id);
        Passersby[index_volunteer]->set_injured_position(target_position);
        Passersby[index_volunteer]->set_step_waiting(0);
        Passersby[index_volunteer]->set_behavior(new Approaching_volunteer);
        //
        int volunteer_id = Passersby[index_volunteer]->get_id();
        if (commitment == true){
            Passersby[index_volunteer]->set_commitment(true);
            //std::cout << "injured Ped_" << target_id << "\tcommitted volunteer Ped_" << volunteer_id <<"\n";
        } else{
            Passersby[index_volunteer]->set_commitment(false);
            //std::cout << "injured Ped_" << target_id << "\tnon-committed volunteer Ped_" << volunteer_id <<"\n";
        }
        //
        Ped_i->add_volunteer(Passersby[index_volunteer]);
        //
        // TEST ONLY
        //std::cout << Ped_i->get_step_current() <<" Ped_"<< Ped_i->get_id() <<" ";
        //std::cout << Ped_i->Ped_AgentType()->print_agent_type() <<" ";
        //std::cout << Passersby[index_volunteer]->get_id() <<" ";
        //std::cout << Passersby[index_volunteer]->get_target_injured_id() <<"\n";
        //
    }
}

