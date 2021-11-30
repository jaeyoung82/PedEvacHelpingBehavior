//
// pedestrian.h
//
//#define _USE_MATH_DEFINES	// should appear before #include <cmath>
#define M_PI            3.14159265358979323846  /* pi */
//
// rounding number to d decimal places
// see: https://stackoverflow.com/questions/1343890/rounding-number-to-2-decimal-places-in-c
#define roundz(x, d)    ((std::floor(((x)*std::pow(10.0, d))+0.5))/std::pow(10.0, d))
#define M_epsilon       0.0001
#define get_min(a,b)    ((a>b)? b:a)
#define get_max(a,b)    ((a<b)? b:a)
//
#ifndef pedestrian_h
#define pedestrian_h
//
#include "vector2D.h"
#include "area.h"
#include "behaviors.h"

// forward declaration
class SocialForceModel;
class InitialVolunteer;
class EvolutionaryGame;

//
// notations? see...
// https://stackoverflow.com/questions/13018189/what-does-m-variable-prefix-mean
//
// Rules for enumerations? see:
// https://www.codesdope.com/cpp-enum/
// https://www.modernescpp.com/index.php/c-core-guidelines-rules-for-enumerations
// https://www.ibm.com/support/knowledgecenter/en/SSLTBW_2.3.0/com.ibm.zos.v2r3.cbclx01/init_enum.htm
//
enum class agent_types{
    passerby,   // 0
    volunteer,  // 1
    injured     // 2
};

class AgentType{
    private:
        enum agent_types my_agent_type;
        int ped_id;

    public:
        AgentType();
        ~AgentType();
        void set_agent_type(agent_types);
        agent_types get_agent_type() const {return my_agent_type;}
        int print_agent_type();
};

class RescuePoint{
    private:
        Pedestrian* Rescuer;
        Vector2D position;
        int rescue_point_id;

    public:
        RescuePoint(int);
        ~RescuePoint();
        void set_rescue_position(Vector2D position_in)  {position = position_in;}
        void set_volunteer(Pedestrian* Ped_volunteer)   {Rescuer = Ped_volunteer;}
        Vector2D get_rescue_position() const            {return position;}
        Pedestrian* Ped_rescuer() const                 {return Rescuer; }
};

class Group{
    private:
        int group_id;
        int n_members;
        int member_id_0;
        int member_id_1;
        int member_id_2;
        Vector2D group_center;
        //std::vector <int> member_id;

    public:
        Group(int);
        ~Group();
        void set_id_memebers(int, int);             // one injured and one volunteer
        void set_id_memebers(int, int, int);        // one injured and two volunteers
        void set_group_center(Vector2D position)    {group_center = position;}
        void set_group_size(int n_peds)             {n_members = n_peds;}
        Vector2D get_group_center() const           {return group_center;}
        int get_group_size() const                  {return n_members;}
};

class Pedestrian{
    //friend class BaseBehavior;
    private:
        int ped_id;
        int area_id;
        int step_current;
        int freq_update1;
        int freq_update2;
        double delta_t;
        int fps;
        double time_current;
        //
        double r_ped;
        double d_members;
        bool remove_me;
        //
        //
        int run_id;
        int n_steps;
        int n_ped;
        //
        // pedestrian kinematics
        Vector2D position;
        Vector2D velocity;
        Vector2D acceleration;
        Vector2D e_i;
        Vector2D ei_ini;    // initial desired walking direction
        Vector2D target;
        double d_target;
        //
        // boundary condition
        std::vector <Area*> Areas;
        //
        // neighbors
        std::vector <Pedestrian*> Neighbors;        // social force model
        std::vector <Pedestrian*> Passerby_near;    // volunteer's dilemma game
        std::vector <Pedestrian*> Peer_near;        // evolutionary game
        std::vector <Pedestrian*> Injuries;         // evolutionary game
        //
        // social force model
        SocialForceModel* SFM;
        //
        // Assigning the initial volunteers
        InitialVolunteer* IniVol;
        //
        // Evolutionary Game model
        EvolutionaryGame* EvolGame;
        int n_game_neighbors;
        int n_neighbor_CC;
        int n_neighbor_CD;
        int n_neighbor_DD;
        int n_neighbor_DC;
        //
        // pedestrian behavior
        BaseBehavior* current_behavior;
        //
        // agent type
        AgentType* Agent_type;
        //
        // injured person
        int n_required_volunteers;
        int n_current_volunteers;
        bool injured_rescued;
        std::vector <Pedestrian*> Volunteers;
        std::vector <RescuePoint*> RescuePoints;
        //
        // group
        int my_group_id;
        //
        //
        // assigning initial volunteers
        double d_iniVol;     // range of initial volunteer assignment
        bool helping_someone;
        bool initial_volunteer;
        bool commitment;
        int target_injured_id;
        Vector2D target_ei;
        Vector2D target_position;
        Vector2D position_injured;
        Pedestrian* Ped_target;
        int step_waiting;
        //
        // Evolutionary game setup
        double  payoff_R;
        double  payoff_P;
        double  payoff_T;
        double  payoff_S;
        bool    play_game;
        double  payoff_now;
        double  beta;           // selection pressure
        //
        double d_evolgame;      // Evolutionary game range parameters
        double l_s;
        //
        //
        // group behavior
        //std::vector <Group*> Groups;
        Group* Group_i;
        bool is_move_group;
        int group_id;

    public:
        Pedestrian(int, Vector2D, std::vector <Area*> Areas_input, double, double, double, double, int, int, int, int);
        virtual ~Pedestrian();
        //
        //
        void initialize_behavior(BaseBehavior* initial_behavior);
        void update(std::vector <Pedestrian*> Pedestrians, int);
        void remove_from_simulation();
        void check_area();
        void check_neighbors(std::vector <Pedestrian*> Pedestrians);
        void prep_EvolGame(std::vector <Pedestrian*> Pedestrians);
        double UniformRnd(int);
        double GaussRnd(double, double, int);
        //
        // access functions
        //
        SocialForceModel*   const Ped_SFM()                     const { return SFM; }
        std::vector <Area*> const Ped_Areas()                   const { return Areas; }
        InitialVolunteer*   const Ped_iniVol()                  const { return IniVol; }
        EvolutionaryGame*   const Ped_EvolGame()                const { return EvolGame; }
        BaseBehavior*       const Ped_Behavior()                const { return current_behavior; }
        Pedestrian*         const Ped_injured()                 const { return Ped_target; }
        std::vector <RescuePoint*>  const Ped_rescue_points()   const { return RescuePoints; }
        std::vector <Pedestrian*>   const Ped_volunteers()      const { return Volunteers; }
        Group*              const Ped_Group()                   const { return Group_i;}
        AgentType*          const Ped_AgentType()               const { return Agent_type;}
        //
        // set functions
        //
        void set_position(Vector2D _position)           {position = _position; }
        void set_velocity(Vector2D _velocity)           {velocity = _velocity; }
        void set_acceleration(Vector2D _acceleration)   {acceleration = _acceleration; }
        void set_ei(Vector2D _ei)                       {e_i = _ei; }
        void reset_agent_type();
        void set_remove()                               {remove_me = true; }
        void reset_behavior();
        void set_behavior(BaseBehavior* new_behavior); // see BaseBehavior.h and behaviors.h
        void set_move_group(bool indicator)             {is_move_group = indicator;}

        //
        // injured person
        void set_n_required_volunteers(int n_required)  {n_required_volunteers = n_required;}
        void set_n_current_volunteers(int n_current)    {n_current_volunteers = n_current;}
        void add_n_current_volunteers()                 {n_current_volunteers += 1;}
        void minus_n_current_volunteers()               {n_current_volunteers -= 1;}
        void set_injured_rescued(bool rescued)          {injured_rescued = rescued;}
        void add_volunteer(Pedestrian* Volunteer)       {Volunteers.push_back(Volunteer);}
        void remove_volunteer(Pedestrian* Volunteer);
        //
        // volunteer
        void set_helping_someone(bool helping)              {helping_someone = helping;}
        void set_initial_volunteer(bool ini)                {initial_volunteer = ini;}
        void set_commitment(bool ini)                       {commitment = ini;}
        void set_target_injured_id(int terget_id)           {target_injured_id = terget_id;}
        void set_injured_position(Vector2D p_injured)       {position_injured = p_injured;}
        void set_target_injured(Pedestrian* Ped_j)          {Ped_target = Ped_j;}
        void set_target_position(Vector2D target)           {target_position = target;}
        void set_target_ei(Vector2D walking_direction)      {target_ei = walking_direction;}
        void set_d_target(double distance)                  {d_target = distance;}
        void set_step_waiting(int n_step_waiting)           {step_waiting = n_step_waiting;}
        //
        // group
        void set_my_group_id(int group_id)                  {my_group_id = group_id;}
        //
        // get functions
        //
        int get_id()                const {return ped_id;}
        Vector2D get_position()     const {return position;}
        Vector2D get_velocity()     const {return velocity;}
        Vector2D get_acceleration() const {return acceleration;}
        Vector2D get_ei()           const {return e_i;}
        bool get_move_group()       const {return is_move_group;}
        int get_group_id()          const {return group_id;}
        double get_payoff_now()     const {return payoff_now;}
        int get_n_game_neighbors()  const {return n_game_neighbors;}
        int get_n_neighbor_CC()     const {return n_neighbor_CC;}
        int get_n_neighbor_CD()     const {return n_neighbor_CD;}
        int get_n_neighbor_DD()     const {return n_neighbor_DD;}
        int get_n_neighbor_DC()     const {return n_neighbor_DC;}
        bool get_play_game()        const {return play_game;}


        bool get_rescued()          const {return injured_rescued;}
        int  get_area_id()          const {return area_id;}
        //
        bool get_remove()           const {return remove_me;}
        //
        double get_rPed()           const {return r_ped;}
        double get_d_members()      const {return d_members;}
        double get_dt()             const {return delta_t;}
        int    get_fps()            const {return fps;}
        int    get_step_current()   const {return step_current;}
        double get_freq_update1()   const {return freq_update1;}
        double get_freq_update2()   const {return freq_update2;}
        double get_bottleneck_up()  const {return Areas[0]->get_crossing_p1(0).y-r_ped;}
        double get_bottleneck_down()const {return Areas[0]->get_crossing_p2(0).y+r_ped;}
        //
        // injured person
        int  get_n_required_volunteers()const {return n_required_volunteers;}
        int  get_n_current_volunteers() const {return n_current_volunteers;}
        bool get_injured_rescued()      const {return injured_rescued;}
        //
        // volunteer
        bool get_helping_someone()              const {return helping_someone;}
        bool get_initial_volunteer()            const {return initial_volunteer;}
        bool get_commitment()                   const {return commitment;}
        int get_target_injured_id()             const {return target_injured_id;}
        Vector2D get_target_ei()                const {return target_ei;}
        Vector2D get_injured_position()         const {return position_injured;}
        Vector2D get_target_position()          const {return target_position;}
        double get_d_target()                   const {return d_target;}
        int get_step_waiting()                  const {return step_waiting;}
        //
        // group
        int get_my_group_id()                   const {return my_group_id;}

};
//static int agent_id_ref = -1;
#endif
