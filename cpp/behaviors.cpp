#include "behaviors.h"
#include "pedestrian.h"
#include "SocialForceModel.h"
#include "InitialVolunteer.h"
#include "EvolutionaryGame.h"

//-----------------------------------------------------
// Passerby behaviors
//-----------------------------------------------------

Egressing_passerby::Egressing_passerby(){
    //std::cout << "this pedestrian is going to egress\n";
    set_behavior_mode(behavior_modes::egressing_passerby);
}

Egressing_passerby::~Egressing_passerby(){

}

void Egressing_passerby::update(){
    //
    // perform Evolutionary Game every update step
    int step_current = Ped->get_step_current();
    //int Ped_id = Ped->get_id();
    //
    int freq_update1 = Ped->get_freq_update1();
    int freq_update2 = Ped->get_freq_update2();
    if ((std::fmod(step_current, freq_update2) == 0)&&(step_current > freq_update1)){
        if (Ped->get_area_id() == 0){
            //
            // this player is D (passerby)
            // player i is D, player j is C
            Ped->Ped_EvolGame()->play_EvolutionaryGame();
        }
    }
    //
    // update egress direction
    this->set_egress_direction();

    // update driving force term
    Ped->Ped_SFM()->set_ei(e_i);
    Ped->Ped_SFM()->update_driving(e_i);
    Ped->set_ei(e_i);

    // update repulsion force from boundary
    Ped->Ped_SFM()->update_repulsionB();

    // update repulsion force from other pedestrians
    Ped->Ped_SFM()->update_CS();

    // update acceleration
    Ped->Ped_SFM()->update_acceleration();

    // update velocity
    Ped->Ped_SFM()->update_velocity();

    // update position
    Ped->Ped_SFM()->update_position();
}

void Egressing_passerby::set_egress_direction(){
    //
    Ped->check_area();
    int Ped_area_id = Ped->get_area_id();
    //
    if (Ped_area_id == 0){
        //double agent_radius = Ped->get_rPed();
        double bottleneck_up = Ped->get_bottleneck_up();
        double bottleneck_down = Ped->get_bottleneck_down();
        double position_x = Ped->get_position().x;
        double position_y = Ped->get_position().y;
        if (position_x <= 0.0){
            if ((position_y < bottleneck_up)&&(position_y > bottleneck_down)){
                // the pedestrian can see the exit
                this->target = Ped->get_position()+Vector2D(1.0, 0);
                this->e_i = Vector2D(1.0, 0.0);
            } else{
                // the pedestrian cannot see the exit, so walk towards the door
                //double bottleneck_center_y = (bottleneck_up+bottleneck_down)*0.5;
                //this->target = Vector2D(-Ped->get_rPed(), bottleneck_center_y);
                this->target = Vector2D(-Ped->get_rPed(), 0.0);
                this->e_i = (this->target-Ped->get_position())/get_max((this->target-Ped->get_position()).length(), M_epsilon);
                //std::cout << "Ped_" << Ped->get_id() <<"\t"<< target.x <<"\t"<< target.y <<"\t"<< e_i.x <<"\t"<< e_i.y <<"\n";
            }
        } else{
            // the pedestrian can see the exit
            this->target = Ped->get_position()+Vector2D(1.0, 0);
            this->e_i = Vector2D(1.0, 0.0);
        }
    } else {
        //std::cout << "Ped_" << Ped->get_id() <<"\t"<< Ped_area_id <<"\n";
        // the pedestrian is in the corridor and can see the exit
        this->target = Ped->get_position()+Vector2D(1.0, 0);
        this->e_i = Vector2D(1.0, 0.0);
    }
    //std::cout << "Ped_" << Ped->get_id() <<"\t"<< this->target.x <<"\t"<< this->target.y <<"\n";
    //std::cout << "Ped_" << Ped->get_id() <<"\t"<< this->e_i.x <<"\t"<< this->e_i.y <<"\n";
}

//-----------------------------------------------------
// Injured person behaviors
//-----------------------------------------------------

// Recruiting behavior---------------------------------
Recruiting_injured::Recruiting_injured(){
    //std::cout << "this pedestrian is waiting to be rescued...\n";
    set_behavior_mode(behavior_modes::recruiting_injured);
}

Recruiting_injured::~Recruiting_injured(){
}

void Recruiting_injured::update(){
    //int step_current = Ped->get_step_current();
    //int Ped_id = Ped->get_id();
    //
    // perform volunteer's dilemma game every update step
    /*
    //
    int freq_update1 = Ped->get_freq_update1(); //
    //int step_update2 = Ped->get_step_update2(); //
    if (step_current == freq_update1){ // one-shot volunteer's dilemma game
        if (Ped->get_area_id() == 0){
            // if the pedestrian is in the room,
            // play volunteer's dilemma game
            if (Ped->get_rescued() == false){
                Ped->Ped_VDG()->play_VolunteerGame();
            }
        }
    }
    */
    //
    if (Ped->get_n_current_volunteers() == Ped->get_n_required_volunteers()){
        // update moving direction
        this->set_moving_direction();

        // update volunteer positions
        this->set_volunteer_positions();

        // switch behavioral mode to "Waiting_injured"
        //Ped->reset_behavior();
        Ped->set_behavior(new Waiting_injured);
        //
        //std::cout << step_current <<"\t"<< Ped_id <<"\t"<< "switch to waiting behavior" <<"\n";
    } else{
        // update moving direction
        this->set_moving_direction();

        // update volunteer positions
        this->set_volunteer_positions();
    }
}

void Recruiting_injured::set_moving_direction(){
    //
    // this function is very similar to Egress_passerby::set_egress_direction()
    //
    Ped->check_area();
    int Ped_area_id = Ped->get_area_id();
    //
    // initialize desired moving direction
    if (Ped_area_id == 0){
        double agent_radius = Ped->get_rPed();
        double bottleneck_up = Ped->get_bottleneck_up()-2.0*agent_radius;
        double bottleneck_down = Ped->get_bottleneck_down()+2.0*agent_radius;
        double position_x = Ped->get_position().x;
        double position_y = Ped->get_position().y;
        if (position_x <= 0.0){
            if ((position_y < bottleneck_up)&&(position_y > bottleneck_down)){
                // the pedestrian can see the exit
                this->target = Ped->get_position()+Vector2D(1.0, 0);
                this->e_i = Vector2D(1.0, 0.0);
                //std::cout << "Ped_" << Ped->get_id() <<"\t"<< target.x <<"\t"<< target.y <<"\t"<< e_i.x <<"\t"<< e_i.y <<"\n";
            } else{
                // the pedestrian cannot see the exit, so walk towards the door
                double temp_target_x = -0.75;
                double temp_target_y = 0.0;
                this->target = Vector2D(temp_target_x, temp_target_y);
                this->e_i = (this->target-Ped->get_position())/get_max((this->target-Ped->get_position()).length(), M_epsilon);
                //std::cout << "Ped_" << Ped->get_id() <<"\t"<< target.x <<"\t"<< target.y <<"\t"<< e_i.x <<"\t"<< e_i.y <<"\n";
            }
            //std::cout << "Ped_" << Ped->get_id() <<"\t"<< target.x <<"\t"<< target.y <<"\t"<< e_i.x <<"\t"<< e_i.y <<"\n";
        } else{
            // the pedestrian can see the exit
            this->target = Ped->get_position()+Vector2D(1.0, 0);
            this->e_i = Vector2D(1.0, 0.0);
        }
    } else {
        //std::cout << "Ped_" << Ped->get_id() <<"\t"<< Ped_area_id <<"\n";
        // the pedestrian is in the corridor and can see the exit
        this->target = Ped->get_position()+Vector2D(1.0, 0);
        this->e_i = Vector2D(1.0, 0.0);
    } // end of desired moving direction initialization
    //
    //
    // update e_i for the injured person
    Ped->set_ei(this->e_i);
    Ped->Ped_SFM()->set_ei(e_i);
    Ped->set_ei(e_i);
    //std::cout << "Ped_" << Ped->get_id() <<"\t"<< this->target.x <<"\t"<< this->target.y <<"\n";
    //std::cout << "Ped_" << Ped->get_id() <<"\t"<< this->e_i.x <<"\t"<< this->e_i.y <<"\n";
}

void Recruiting_injured::set_volunteer_positions(){
    //
    // assign desired position for volunteers
    //
    // clockwise (CW) rotation matrix
    // [cos_theta, -1*sin_theta;
    //  sin_theta,    cos_theta]
    //
    // counterclockwise (CCW) rotation matrix
    // [   cos_theta, sin_theta;
    //  -1*sin_theta, cos_theta]
    //
    // also refer to
    // https://en.wikipedia.org/wiki/Rotation_matrix
    // Moussaid et al. PLOS 2010
    //
    double theta_rotate_rad = 1.26;
    double alpha_rad = 0.31;
    double ei_x = Ped->get_ei().x;
    double ei_y = Ped->get_ei().y;
    double injured_x = Ped->get_position().x;
    double injured_y = Ped->get_position().y;
    double d_members = Ped->get_d_members();
    //double x_ref = injured_x+d_members*ei_x;
    //double y_ref = injured_y+d_members*ei_y;
    double ref_delta_x = d_members*ei_x;
    double ref_delta_y = d_members*ei_y;
    //
    // helper1, CCW
    double x_helper1 = injured_x+cos(theta_rotate_rad)*ref_delta_x+sin(theta_rotate_rad)*ref_delta_y;
    double y_helper1 = injured_y+(-1.0)*sin(theta_rotate_rad)*ref_delta_x+cos(theta_rotate_rad)*ref_delta_y;
    Vector2D position_helper1 = Vector2D(x_helper1, y_helper1);
    //
    // helper2, CW
    double x_helper2 = injured_x+cos(theta_rotate_rad)*ref_delta_x+(-1.0)*sin(theta_rotate_rad)*ref_delta_y;
    double y_helper2 = injured_y+sin(theta_rotate_rad)*ref_delta_x+cos(theta_rotate_rad)*ref_delta_y;
    Vector2D position_helper2 = Vector2D(x_helper2, y_helper2);
    //
    // assign desired position for volunteers
    if (Ped->get_n_current_volunteers() >= 1){
    Ped->Ped_rescue_points()[0]->set_rescue_position(position_helper1);
    Ped->Ped_rescue_points()[0]->set_volunteer(Ped->Ped_volunteers()[0]);
    Ped->Ped_volunteers()[0]->set_target_position(position_helper1);
        if (Ped->get_n_current_volunteers() == 2){
            Ped->Ped_rescue_points()[1]->set_rescue_position(position_helper2);
            Ped->Ped_rescue_points()[1]->set_volunteer(Ped->Ped_volunteers()[1]);
            Ped->Ped_volunteers()[1]->set_target_position(position_helper2);
        }
    }
}

// Waiting behavior------------------------------------
Waiting_injured::Waiting_injured(){
    set_behavior_mode(behavior_modes::waiting_injured);
    //
    // initialize waiting time
    step_waiting = 0;
}

Waiting_injured::~Waiting_injured(){

}

void Waiting_injured::update(){
    //
    int step_current = Ped->get_step_current();
    int Ped_id = Ped->get_id();
    int n_volunteers = Ped->get_n_current_volunteers();
    //
    // update moving direction
    this->set_moving_direction();
    //
    // update volunteer positions
    this->set_volunteer_positions();
    //
    // check distance between the volunteers and their desired positions
    // then check the waiting time
    double d_threshold = 0.05;
    bool is_helpers_arrived = false;
    int fps = Ped->get_fps();
    int t_threshold = 3; // seconds
    int step_threshold = fps*t_threshold;
    //
    if (Ped->get_n_required_volunteers() == 1){
        //
        Vector2D point_1  = Ped->Ped_rescue_points()[0]->get_rescue_position();
        Vector2D helper_1 = Ped->Ped_volunteers()[0]->get_position();
        int helper1_id    = Ped->Ped_volunteers()[0]->get_id();
        double d_1 = (point_1-helper_1).length();
        if (d_1 < d_threshold){
            is_helpers_arrived = true;
            step_waiting += 1;
            if (step_waiting == step_threshold){
                //std::cout << step_current <<"\t"<< Ped_id <<"\t"<< "volunteer # "<< helper1_id << " arrived\n";
                Ped->set_behavior(new Egressing_injured);
                Ped->Ped_Group()->set_id_memebers(Ped_id, helper1_id);
                Ped->set_my_group_id(Ped_id);
                Ped->Ped_volunteers()[0]->set_step_waiting(step_waiting);
                Ped->Ped_volunteers()[0]->set_my_group_id(Ped_id);
            }
        }
    } else if (Ped->get_n_required_volunteers() == 2){
        //
        if (Ped->get_n_current_volunteers() >= 1){
            Vector2D point_1  = Ped->Ped_rescue_points()[0]->get_rescue_position();
            Vector2D helper_1 = Ped->Ped_volunteers()[0]->get_position();
            int helper1_id    = Ped->Ped_volunteers()[0]->get_id();
            double d_1 = (point_1-helper_1).length();
            //
            if (Ped->get_n_current_volunteers() == 2){
                //
                Vector2D point_2  = Ped->Ped_rescue_points()[1]->get_rescue_position();
                Vector2D helper_2 = Ped->Ped_volunteers()[1]->get_position();
                int helper2_id    = Ped->Ped_volunteers()[1]->get_id();
                double d_2 = (point_2-helper_2).length();
                //
                if ((d_1 < d_threshold)&&(d_2 < d_threshold)){
                    is_helpers_arrived = true;
                    step_waiting += 1;
                    if (step_waiting == step_threshold){
                        //std::cout << step_current <<"\t"<< Ped_id <<"\t"<< "volunteers # "<< helper1_id <<" and # "<< helper2_id << " arrived\n";
                        Ped->set_behavior(new Egressing_injured);
                        Ped->set_my_group_id(Ped_id);
                        Ped->Ped_volunteers()[0]->set_step_waiting(step_waiting);
                        Ped->Ped_volunteers()[1]->set_step_waiting(step_waiting);
                        Ped->Ped_volunteers()[0]->set_my_group_id(Ped_id);
                        Ped->Ped_volunteers()[1]->set_my_group_id(Ped_id);
                        // Ped->Ped_Group()->set_id_memebers(Ped_id, helper1_id, helper2_id); // <-- FIX HERE!
                    }
                }
            }
        }
        //
    }
}

void Waiting_injured::set_moving_direction(){
    //
    // this function is very similar to Recruiting_injured::set_moving_direction()
    //
    Ped->check_area();
    int Ped_area_id = Ped->get_area_id();
    //
    // initialize desired moving direction
    if (Ped_area_id == 0){
        double agent_radius = Ped->get_rPed();
        double bottleneck_up = Ped->get_bottleneck_up()-2.0*agent_radius;
        double bottleneck_down = Ped->get_bottleneck_down()+2.0*agent_radius;
        double position_x = Ped->get_position().x;
        double position_y = Ped->get_position().y;
        if (position_x <= 0.0){
            if ((position_y < bottleneck_up)&&(position_y > bottleneck_down)){
                // the pedestrian can see the exit
                this->target = Ped->get_position()+Vector2D(1.0, 0);
                this->e_i = Vector2D(1.0, 0.0);
            } else{
                // the pedestrian cannot see the exit, so walk towards the door
                double temp_target_x = -0.75;
                double temp_target_y = 0.0;
                this->target = Vector2D(temp_target_x, temp_target_y);
                this->e_i = (this->target-Ped->get_position())/get_max((this->target-Ped->get_position()).length(), M_epsilon);
            }
        } else{
            // the pedestrian can see the exit
            this->target = Ped->get_position()+Vector2D(1.0, 0);
            this->e_i = Vector2D(1.0, 0.0);
        }
    } else {
        // the pedestrian is in the corridor and can see the exit
        this->target = Ped->get_position()+Vector2D(1.0, 0);
        this->e_i = Vector2D(1.0, 0.0);
    } // end of desired moving direction initialization
    //
    // update e_i for the injured person
    Ped->set_ei(this->e_i);
    Ped->Ped_SFM()->set_ei(e_i);
    Ped->set_ei(e_i);
}

void Waiting_injured::set_volunteer_positions(){
    //
    // this function is very similar to Recruiting_injured::set_volunteer_positions()
    //
    //
    // assign desired position for volunteers
    //
    // clockwise (CW) rotation matrix
    // [cos_theta, -1*sin_theta;
    //  sin_theta,    cos_theta]
    //
    // counterclockwise (CCW) rotation matrix
    // [   cos_theta, sin_theta;
    //  -1*sin_theta, cos_theta]
    //
    // also refer to
    // https://en.wikipedia.org/wiki/Rotation_matrix
    // Moussaid et al. PLOS 2010
    //
    double theta_rotate_rad = 1.26;
    double alpha_rad = 0.31;
    double ei_x = Ped->get_ei().x;
    double ei_y = Ped->get_ei().y;
    double injured_x = Ped->get_position().x;
    double injured_y = Ped->get_position().y;
    double d_members = Ped->get_d_members();
    double ref_delta_x = d_members*ei_x;
    double ref_delta_y = d_members*ei_y;
    //
    // helper1, CCW
    double x_helper1 = injured_x+cos(theta_rotate_rad)*ref_delta_x+sin(theta_rotate_rad)*ref_delta_y;
    double y_helper1 = injured_y+(-1.0)*sin(theta_rotate_rad)*ref_delta_x+cos(theta_rotate_rad)*ref_delta_y;
    Vector2D position_helper1 = Vector2D(x_helper1, y_helper1);
    //
    // helper2, CW
    double x_helper2 = injured_x+cos(theta_rotate_rad)*ref_delta_x+(-1.0)*sin(theta_rotate_rad)*ref_delta_y;
    double y_helper2 = injured_y+sin(theta_rotate_rad)*ref_delta_x+cos(theta_rotate_rad)*ref_delta_y;
    Vector2D position_helper2 = Vector2D(x_helper2, y_helper2);
    //
    // assign desired position for volunteers
    Ped->Ped_rescue_points()[0]->set_rescue_position(position_helper1);
    Ped->Ped_rescue_points()[1]->set_rescue_position(position_helper2);
    //
    // get_n_current_volunteers or get_n_required_volunteers ???
    if (Ped->get_n_required_volunteers() == 1){
        // if there is one volunteer
        Vector2D position_volunteer = Ped->Ped_volunteers()[0]->get_position();
        double d_left_square = (position_volunteer-position_helper1).length2();
        double d_right_square = (position_volunteer-position_helper1).length2();
        if (d_left_square <= d_right_square){
            // go to the left side of the injured
            Ped->Ped_rescue_points()[0]->set_volunteer(Ped->Ped_volunteers()[0]);
            Ped->Ped_volunteers()[0]->set_target_position(position_helper1);
        } else{
            // go to the right side of the injured
            Ped->Ped_rescue_points()[1]->set_volunteer(Ped->Ped_volunteers()[0]);
            Ped->Ped_volunteers()[0]->set_target_position(position_helper2);
        }
    } else if (Ped->get_n_required_volunteers() == 2){
        // if there are two volunteers
        Ped->Ped_rescue_points()[0]->set_volunteer(Ped->Ped_volunteers()[0]);
        Ped->Ped_rescue_points()[1]->set_volunteer(Ped->Ped_volunteers()[1]);
        Ped->Ped_volunteers()[0]->set_target_position(position_helper1);
        Ped->Ped_volunteers()[1]->set_target_position(position_helper2);
    }
}

// Egressing behavior-------------------------------------
Egressing_injured::Egressing_injured(){
    set_behavior_mode(behavior_modes::egressing_injured);
}

Egressing_injured::~Egressing_injured(){

}

void Egressing_injured::update(){
    //
    int step_current = Ped->get_step_current();
    int Ped_id = Ped->get_id();
    int my_group_id = Ped->get_my_group_id();
    //
    // update moving direction
    this->set_moving_direction();
    Vector2D walking_direction = Ped->get_ei();
    if (Ped->get_n_current_volunteers() == 1){
        Ped->Ped_volunteers()[0]->set_target_ei(walking_direction);
    }else if (Ped->get_n_current_volunteers() == 2){
        Ped->Ped_volunteers()[0]->set_target_ei(walking_direction);
        Ped->Ped_volunteers()[1]->set_target_ei(walking_direction);
    }
    /*
    //
    // update driving force term
    Ped->Ped_SFM()->update_driving_group(walking_direction);
    //
    // update repulsion force from boundary
    Ped->Ped_SFM()->update_repulsionB();
    //
    // update repulsion force from other pedestrians
    Ped->Ped_SFM()->update_CS_group();
    //
    // update acceleration
    Ped->Ped_SFM()->update_acceleration();
    //
    // update velocity
    Ped->Ped_SFM()->update_velocity();
    //
    // update position
    Ped->Ped_SFM()->update_position();
    */
}

void Egressing_injured::set_moving_direction(){
    //
    // this function is very similar to Recruiting_injured::set_moving_direction()
    //
    Ped->check_area();
    int Ped_area_id = Ped->get_area_id();
    //
    // initialize desired moving direction
    if (Ped_area_id == 0){
        double agent_radius = Ped->get_rPed();
        double bottleneck_up = Ped->get_bottleneck_up()-2.0*agent_radius;
        double bottleneck_down = Ped->get_bottleneck_down()+2.0*agent_radius;
        double position_x = Ped->get_position().x;
        double position_y = Ped->get_position().y;
        if (position_x <= 0.0){
            if ((position_y < bottleneck_up)&&(position_y > bottleneck_down)){
                // the pedestrian can see the exit
                this->target = Ped->get_position()+Vector2D(1.0, 0);
                this->e_i = Vector2D(1.0, 0.0);
            } else{
                // the pedestrian cannot see the exit, so walk towards the door
                double temp_target_x = -0.75;
                double temp_target_y = 0.0;
                this->target = Vector2D(temp_target_x, temp_target_y);
                this->e_i = (this->target-Ped->get_position())/get_max((this->target-Ped->get_position()).length(), M_epsilon);
            }
        } else{
            // the pedestrian can see the exit
            this->target = Ped->get_position()+Vector2D(1.0, 0);
            this->e_i = Vector2D(1.0, 0.0);
        }
    } else {
        // the pedestrian is in the corridor and can see the exit
        this->target = Ped->get_position()+Vector2D(1.0, 0);
        this->e_i = Vector2D(1.0, 0.0);
    } // end of desired moving direction initialization
    //
    // update e_i for the injured person
    Ped->set_ei(this->e_i);
    Ped->Ped_SFM()->set_ei(e_i);
    Ped->set_ei(e_i);
}

//-----------------------------------------------------
// Volunteer behaviors
//-----------------------------------------------------

// Approaching behavior------------------------------------
//-----------------------------------------------------
Approaching_volunteer::Approaching_volunteer(){
    set_behavior_mode(behavior_modes::approaching_volunteer);
}

Approaching_volunteer::~Approaching_volunteer(){

}

void Approaching_volunteer::update(){
    //
    int step_current = Ped->get_step_current();
    int Ped_id = Ped->get_id();
    //
    // check whether the target exists
    bool target_exists = false;
    int target_id = Ped->get_target_injured_id();
    if (target_id >= 0){
        target_exists = true;
    }
    //
    int freq_update1 = Ped->get_freq_update1();
    int freq_update2 = Ped->get_freq_update2();
    bool initial_volunteer = Ped->get_initial_volunteer();
    bool committed = Ped->get_commitment();
    //
    if ((std::fmod(step_current, freq_update2) == 0)&&(step_current > freq_update1)){
        if (Ped->get_area_id() == 0){
            //
            // this player is C (volunteer)
            //
            if (committed == false){
                Ped->Ped_EvolGame()->play_EvolutionaryGame();
            }
            //
        }
    }
    //
    // check waiting time
    int fps = Ped->get_fps();
    int t_threshold = 5; // seconds
    int step_threshold = fps*t_threshold;
    int step_waiting = Ped->get_step_waiting();
    if (step_waiting == step_threshold){
        //std::cout << step_current <<"\t"<< "volunteer # "<< Ped_id <<" is ready to move\n";
        // switch behavioral mode to "Egressing_volunteer"
        Ped->set_behavior(new Egressing_volunteer);
    }
    //
    if (target_exists == true){
        //
        // check desired walking direction
        this->target = Ped->get_target_position();
        //std::cout << Ped_id <<"\t"<< target.x <<"\t"<< target.y << "\n";
        //
        Vector2D displacement = target-Ped->get_position();
        double delta_x = displacement.x;
        double delta_y = displacement.y;
        double d_target = get_max(displacement.length(), M_epsilon);
        this->e_i = Vector2D(delta_x/d_target, delta_y/d_target);
        //
        Ped->set_ei(this->e_i);
        Ped->Ped_SFM()->set_ei(e_i);
        Ped->set_ei(e_i);
        //
        // check distance to the target. if close enough, change behavior model, otherwise update movement
        //
        // update driving force term
        Ped->Ped_SFM()->update_approaching(this->target);
        //
        // update repulsion force from boundary
        Ped->Ped_SFM()->update_repulsionB();
        //
        // update repulsion force from other pedestrians
        Ped->Ped_SFM()->update_CS();
        //
        // update acceleration
        Ped->Ped_SFM()->update_acceleration();
        //
        // update velocity
        Ped->Ped_SFM()->update_velocity();
        //
        // update position
        Ped->Ped_SFM()->update_position();
    }
    //
}

// Egressing behavior------------------------------------
Egressing_volunteer::Egressing_volunteer(){
    set_behavior_mode(behavior_modes::egressing_volunteer);
}

Egressing_volunteer::~Egressing_volunteer(){

}

void Egressing_volunteer::update(){
    //
    int step_current = Ped->get_step_current();
    int Ped_id = Ped->get_id();
    int injured_id = Ped->get_target_injured_id();
    int my_group_id = Ped->get_my_group_id();
    //Ped->set_group_id(injured_id);
    //
    //std::cout << step_current <<"\t"<< "Egressing mode -- volunteer # "<< Ped_id <<" is ready to move\n";
    //

    /*
    // update driving force term
    //int injured_id = Ped->get_target_injured_id();
    Vector2D walking_direction = Ped->get_target_ei();
    Ped->Ped_SFM()->update_driving_group(walking_direction);
    //
    // update repulsion force from boundary
    Ped->Ped_SFM()->update_repulsionB();
    //
    // update repulsion force from other pedestrians
    //Ped->Ped_SFM()->update_CS();
    //
    // update acceleration
    Ped->Ped_SFM()->update_acceleration();
    //
    // update velocity
    Ped->Ped_SFM()->update_velocity();
    //
    // update position
    Ped->Ped_SFM()->update_position();
    */
}
