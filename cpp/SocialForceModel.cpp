#include "SocialForceModel.h"
#include "pedestrian.h"
#include "area.h"

SocialForceModel::SocialForceModel(Pedestrian* Ped_input, std::vector <Area*> Areas_input):
    Ped_i(Ped_input), Areas(Areas_input)
{
    //
    // SFM parameters
    //
    ped_id = Ped_i->get_id();
    delta_t = 0.05;
    r_ped = Ped_i->get_rPed();
    d_members = Ped_i->get_d_members();
    speed_max = 2.0;
    epsilon = 0.01*0.01;
    //
    // driving force
    vd = 1.2;
    vd_group = 0.6; // see Rahouti et al. SafetySci 2020
    tau = 0.5;
    T_a = 1.0; // 0.5 or 1.0
    //
    // repulsion-interpersonal
    C_p = 3.0;
    l_p = 0.3;
    d_cutoff_P = 3.0;
    lambda_ij = 0.25;
    k_n = 50.0;
    k_t = 25.0;
    reduce_factor_Cp = 0.5; // 0.5, 0.25
    reduce_factor_lp = 4.0; // 4.0, 2.5
    //
    // attraction interaction
    C_r0 = 0.7; // 0.8 or 0.7???
    r_0 = 0.35;
    //
    // repulsion-boundary
    C_b = 10.0;
    l_b = 0.1;
    d_cutoff_B = 1.0;
    //
    // SFM vectors
    e_i = Vector2D(0.0, 0.0);
    driving = Vector2D(0.0, 0.0);
    repulsion_B = Vector2D(0.0, 0.0);
    repulsion_P = Vector2D(0.0, 0.0);
    attraction = Vector2D(0.0, 0.0);
    acceleration = Ped_i->get_acceleration();
    velocity = Ped_i->get_velocity();
    position = Ped_i->get_position();
    //
    // random number seed
    step_current = Ped_i->get_step_current();
    rnd_ini = 1;
}

SocialForceModel::~SocialForceModel(){

}

void SocialForceModel::update_SFM(){

}

void SocialForceModel::update_ei(){

}

void SocialForceModel::update_driving(){
    // update driving force term
    this->driving = (e_i*vd-velocity)/tau;
}

void SocialForceModel::update_approaching(Vector2D target){
    // modified driving force term for approaching a location
    //
    Vector2D displacement = target-Ped_i->get_position();
    double delta_x = displacement.x;
    double delta_y = displacement.y;
    double d_target = get_max(displacement.length(), epsilon);
    Ped_i->set_d_target(d_target);
    double speed_approaching = get_min(vd, d_target/T_a);
    Vector2D walking_direction = Vector2D(delta_x/d_target, delta_y/d_target);
    /*
    if (d_target < this->r_ped){
        this->driving = (walking_direction*speed_approaching-velocity)/tau;
    } else{
        this->driving = (walking_direction*vd-velocity)/tau;
    }
    */
    this->driving = (walking_direction*speed_approaching-velocity)/tau;
}

void SocialForceModel::update_driving(Vector2D walking_direction){
    //
    // update driving force term
    this->driving = (walking_direction*vd-velocity)/tau;
    //
    //std::cout << Ped_i->get_id() <<"\t"<< driving.x <<"\t"<< driving.y <<"\t"<< driving.length() <<"\n";
}

void SocialForceModel::update_driving_group(Vector2D walking_direction){
    // update driving force term
    this->driving = (walking_direction*vd_group-velocity)/tau;
}

void SocialForceModel::update_neighbors(std::vector <Pedestrian*> Pedestrians_neighbor){
    this->Neighbors.clear();
    for (unsigned int j = 0; j < Pedestrians_neighbor.size(); j++){
        this->Neighbors.push_back(Pedestrians_neighbor[j]);
    }
}

void SocialForceModel::update_CS(){
    this->repulsion_P = Vector2D(0.0, 0.0);
    double v_i = Ped_i->get_velocity().length();
    bool is_volunteer = Ped_i->get_helping_someone();
    double d_target = Ped_i->get_d_target();
    //
    for (unsigned int j = 0; j < Neighbors.size(); j++) {
        Vector2D delta_ij = Ped_i->get_position() - Neighbors[j]->get_position();
        double d_ij = get_max(delta_ij.length(), M_epsilon);
        double cos_phi = e_i.dot(delta_ij)*(-1.0)/d_ij;
        if (v_i > 0.0){cos_phi = Ped_i->get_velocity().dot(delta_ij)*(-1.0)/(v_i*d_ij);}
        cos_phi = get_max(get_min(cos_phi, 1.0), -1.0);
        double w_ij = get_min((lambda_ij+(1.0-lambda_ij)*(1.0+cos_phi)*0.5), 1.0);
        bool neighbor_is_volunteer = Neighbors[j]->get_helping_someone();
        //
        if (is_volunteer == true){
            // if Ped_i is a volunteer approaching to the injured person
            bool reduce_repulsion = false;
            bool no_repulsion = false;
            bool is_target = false;
            bool is_pair   = false;
            bool is_volunteer2 = false;
            bool near_target = false;
            bool is_passerby = false;
            //
            if (Neighbors[j]->get_id() == Ped_i->get_target_injured_id()){
                // the injured person whom this pedestrian is going to help
                no_repulsion = true;
                is_target = true;
            }
            if (Neighbors[j]->get_target_injured_id() == Ped_i->get_target_injured_id()){
                // the volunteer who is going to rescue the same injured person
                reduce_repulsion = true;
                is_pair = true;
            }
            if (Neighbors[j]->get_helping_someone() == true){
                // other volunteers
                reduce_repulsion = true;
                is_volunteer2 = true;
            }
            if (Neighbors[j]->Ped_AgentType()->get_agent_type() == agent_types::passerby){
                // passerby: Neighbors[j]->get_agent_type() == 0
                is_passerby = true;
            }
            if (d_target < 0.5){
                near_target = true;
            }
            //
            if ((reduce_repulsion == true)&&(is_target == false)){
                if ((is_pair == false)&&(near_target == false)){
                    if (no_repulsion == false){
                        // other volunteers
                        this->repulsion_P += (delta_ij/d_ij)*(reduce_factor_Cp*C_p)*exp((2.0*r_ped-d_ij)/(reduce_factor_lp*l_p))*w_ij; // interpersonal repulsion term
                        //this->repulsion_P += (delta_ij/d_ij)*(reduce_factor_Cp*C_p)*exp((2.0*r_ped-d_ij)/(l_p))*w_ij; // interpersonal repulsion term
                    }
                }
            }
            //
            /*
            //if ((reduce_repulsion == true)&&(is_pair == true)){
            //if (reduce_repulsion == true){
            if ((reduce_repulsion == true)&&(is_target == false)){
                // f_0 = 0.5*C_p*np.exp((2.0*r_ped-d_ij)/(4.0*l_p))
                if ((is_pair == false)&&(near_target == false)){
                    if (no_repulsion == false){
                        this->repulsion_P += (delta_ij/d_ij)*(reduce_factor_Cp*C_p)*exp((2.0*r_ped-d_ij)/(reduce_factor_lp*l_p))*w_ij; // interpersonal repulsion term
                    }
                }
            }
            */
            //
            if (is_passerby == true){
                //this->repulsion_P += (delta_ij/d_ij)*(C_p)*exp((2.0*r_ped-d_ij)/(l_p))*w_ij; // interpersonal repulsion term
                this->repulsion_P += (delta_ij/d_ij)*(0.5*C_p)*exp((2.0*r_ped-d_ij)/(l_p))*w_ij; // interpersonal repulsion term
            }
            // if Neighbor[j] is the person whom Ped_i is going to rescue, no repulsion force is computed at this time
        } else{
            // passerby
            //this->repulsion_P += (delta_ij/d_ij)*C_p*exp((2.0*r_ped-d_ij)/l_p)*w_ij; // interpersonal repulsion term
            //
            if (neighbor_is_volunteer == true){
                // the neighbor is a volunteer
                //this->repulsion_P += (delta_ij/d_ij)*(1.5*C_p)*exp((2.0*r_ped-d_ij)/(1.5*l_p))*w_ij; // interpersonal repulsion term
                this->repulsion_P += (delta_ij/d_ij)*C_p*exp((2.0*r_ped-d_ij)/l_p)*w_ij; // interpersonal repulsion term
            } else{
                this->repulsion_P += (delta_ij/d_ij)*C_p*exp((2.0*r_ped-d_ij)/l_p)*w_ij; // interpersonal repulsion term
            }
        }
        //
        if (d_ij < (2.0*r_ped+epsilon)){
            // normal direction
            this->repulsion_P += (delta_ij/d_ij)*k_n*(2.0*r_ped+epsilon-d_ij);
            // tangential direction
            Vector2D t_ij = Vector2D (-e_i.y, e_i.x); // tangential vector; see Helbing RMP 2001
            double dot_product = std::max(((Neighbors[j]->get_velocity()-Ped_i->get_velocity()).dot(t_ij)), 0.01);
            this->repulsion_P += t_ij*dot_product*k_t*(2.0*r_ped+epsilon-d_ij);
            //this->repulsion_P += t_ij*((Neighbors[j]->get_velocity()-Ped_i->get_velocity()).dot(t_ij))*k_t*(2.0*r_ped+epsilon-d_ij);
        }
    } // end of interpersonal repulsion calculation
}

void SocialForceModel::update_repulsionB(){
    //
    this->repulsion_B = Vector2D(0.0, 0.0);       // initialize repulsion force value
    bool is_volunteer = Ped_i->get_helping_someone();
    bool near_target = false;
    bool repulsion_active = false;
    //
    if (is_volunteer == true){
        double d_target = Ped_i->get_d_target();
        if (d_target < 0.2){
            near_target = true;
        }
    }
    //
    Ped_i->check_area();
    int Ped_area_id = Ped_i->get_area_id();
    int n_walls = Areas[Ped_area_id]->get_n_walls();
    for (int i = 0; i < n_walls; i++){
        // check whether the boundary type corresponds to the area type
        Vector2D displacement = position - Areas[Ped_area_id]->get_nearest_point(i, position);
        double distance = get_max(displacement.length(), epsilon);
        if (distance <= d_cutoff_B){
            if ((is_volunteer == true)&&(near_target == false)){
                repulsion_active = true;
            }
            if (is_volunteer == false){
                repulsion_active = true;
            }
            //
            if (repulsion_active == true){
                Vector2D displacement_unit = displacement/distance;
                this->repulsion_B += displacement_unit*exp(-(distance-r_ped)/l_b);
            }
        }
    }
}

void SocialForceModel::update_attraction(Vector2D target_ped){
    //
    Vector2D delta_ij = Ped_i->get_position() - target_ped;
    double d_ij = get_max(delta_ij.length(), M_epsilon);
    Vector2D e_ij = delta_ij/d_ij;
    attraction = e_ij*(C_r0/r_0)*((r_0/d_ij)*(r_0/d_ij)-1.0);
}

void SocialForceModel::update_acceleration(){
    // check pedestrian type
    //bool is_volunteer = Ped_i->get_helping_someone();
    bool is_passerby = false;
    if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::passerby){
        is_passerby = true;
    }
    // update acceleration for SFM
    this->acceleration = this->driving + this->repulsion_B + this->repulsion_P;
    //
    if (Ped_i->get_helping_someone() == true){
        this->acceleration = this->driving + this->repulsion_B + this->repulsion_P;
    }
    //
    // add random noise
    rnd_ini += 1;
    double noise_avg = 0.0;
    double noise_sd = 0.1;
    //if (is_volunteer = false){
    if (is_passerby == true){
        //this->acceleration.x += GaussRnd(noise_avg, noise_sd);
        //this->acceleration.y += GaussRnd(noise_avg, noise_sd);
        this->acceleration.x += Ped_i->GaussRnd(noise_avg, noise_sd, rnd_ini);
        rnd_ini += 1;
        this->acceleration.y += Ped_i->GaussRnd(noise_avg, noise_sd, rnd_ini);
    }
    /*
    // add attractive interaction term if it exists
    if (Ped_i->get_helping_someone() == true){
        this->acceleration += this->attraction;
    }
    */
    // update acceleration for Ped_i
    Ped_i->set_acceleration(this->acceleration);
}

void SocialForceModel::update_velocity(){
    //
    // limit the new speed up to the maximum speed
    Vector2D velocity_temp = Ped_i->get_velocity()+this->acceleration*delta_t;
    double w_i = get_max(velocity_temp.length(), epsilon);  // temporary value
    Vector2D walking_direction = velocity_temp/w_i;         // update walking direction vector
	double fm = 0.0;
	double p = 8.0;
    //
    if (w_i < speed_max){
        fm = exp(1.0-1.0/(1.0-std::pow((w_i/speed_max), 2.0*p)));
    } else{
        fm = 0.0;
    }
    double speed_new = w_i*fm+w_i*((1.0-fm)*speed_max/(w_i+epsilon));
    //
    // update velocity for SFM
    this->velocity = walking_direction*speed_new;    // Basic SFM, limit the velocity
    //
    // update velocity for Ped_i
    Ped_i->set_velocity(this->velocity);
}

void SocialForceModel::update_position(){
    //
    // update position for SFM
    this->position = this->position+this->velocity*delta_t;
    //
    // update position for Ped_i
    Ped_i->set_position(this->position);
    double corridor_right = Areas[1]->get_crossing_p1(0).x;
    if (this->position.x > corridor_right){
        if (Ped_i->Ped_AgentType()->get_agent_type() == agent_types::passerby){
            // passersby
            // Ped_i->set_remove();
            Ped_i->remove_from_simulation();
        }
    }
}
