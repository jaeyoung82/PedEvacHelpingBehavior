//
// implemented finite state machine
// see examples:
// https://blog.naver.com/ssongmina/221102760580
// https://github.com/deepseasw/WestWorld/tree/master/Src
//
#include "scenario.h"
//
// using namespace std;

std::string ShowTime(const time_t & time){
	// see http://stackoverflow.com/questions/997946/how-to-get-current-time-and-date-in-c
	struct tm * now = localtime(&time);
    std::stringstream current_time;
    current_time << asctime(now); // show current time
    return current_time.str();
}

int main(int argc, char *argv[]){
    //
    // new configuration: using command line arguments
    //
    // required inputs:
    // payoff_T (temptation)
    // payoff_S (sucker's payoff)
    // beta     (selection pressure)
    // value_ls (sensory range)
    //
    //
    // other input parameters
    //
    //int n_ped = 200;        // the number of pedestrians: default = 100
    //int n_injuries = 10;    // the number of injured persons: default  = 10
    //double value_ls = 3.0;  // sensory range
    //
    // iteration setup
    //int n_iterations = 100;  // max 30
    int run_id_begin = 0;   // 0
    //
    // TEST ONLY
    //
    //
    // run scenarios
    /*
    //int run_id = 0;
    double payoff_T = 0.5;
    double payoff_S = -0.5;
    double value_Q = 0.0;
    double value_ls = 3.0;
    //int n_committed = 0;
    //Scenario RoomEvac(run_id, n_ped, n_injuries, n_committed, payoff_T, payoff_S, value_Q, value_k, value_ls); // initialize RoomEvac
    //
    for (int n_committed = 0; n_committed <= 10; n_committed++){
        for (int i = 0; i < 1; i++){
            Scenario RoomEvac(i, n_ped, n_injuries, n_committed, payoff_T, payoff_S, value_Q, value_k, value_ls); // initialize RoomEvac
            RoomEvac.runScenario();
            std::cout <<"\n";
        }
    }
    return 0;
    */
    //
    /*
    int n_argc =6; // the number of required arguments
    if (argc != n_argc){
        std::cerr << "Usage:\n" << "test.exe" << "   argument_1   argument_2   argument_3   argument_4  argument_5\n\n";
        std::cout << "Required arguments:\n";
        std::cout << "argument_1: value_ls (sensory range) = [3, 15]\n";
        std::cout << "argument_2: value_Q  (altruism strength) = [0, 0.5]\n";
        std::cout << "argument_3: payoff_T (temptation) = [0.1, 2]\n";
        std::cout << "argument_4: payoff_S (sucker's payoff) = [-0.1, -1]\n";
        std::cout << "argument_5: run_id\n\n";
        std::cout << "Parameter values already set in the code:\n";
        std::cout << "# ped = "         << n_ped <<"\n";
        std::cout << "# injuries = "    << n_injuries <<"\n";
        std::cout << "value_k = "       << value_k <<"\n";
        return 1;
    }
    else{
        // all the required inputs are provided
        // argc == n_argc
        //
        //
        double value_ls = atof(argv[1]); // sensory range
        double value_Q  = atof(argv[2]); // altruism strength
        double payoff_T = atof(argv[3]); // temptation
        double payoff_S = atof(argv[4]); // sucker's payoff
        int run_id      = atof(argv[5]); // run_id
        //
        // open a log file
        std::stringstream filename_log;
        filename_log << "log.txt";
        std::ofstream write_log(filename_log.str().c_str(), std::ios_base::out|std::ios_base::app);
        //
        // run scenarios
        //
        //
        std::cout <<"k = "      << value_k  <<" ";
        std::cout <<"l_s = "    << value_ls <<" ";
        std::cout <<"Q = "      << value_Q  <<" ";
        std::cout <<"T = "      << payoff_T <<" ";
        std::cout <<"S = "      << payoff_S <<" ";
        std::cout <<"run_id = " << run_id   <<"\n";
        //
        write_log <<"k = "      << value_k  <<" ";
        write_log <<"l_s = "    << value_ls <<" ";
        write_log <<"Q = "      << value_Q  <<" ";
        write_log <<"T = "      << payoff_T <<" ";
        write_log <<"S = "      << payoff_S <<" ";
        write_log <<"run_id = " << run_id   <<"\n";
        //
        // run scenarios
        Scenario RoomEvac(run_id, n_ped, n_injuries, n_committed, payoff_T, payoff_S, value_Q, value_k, value_ls); // initialize RoomEvac
        RoomEvac.runScenario();
        //
        return 0;
    }
    */
    //
    int n_argc = 9; // the number of required arguments
    if (argc != n_argc){
        std::cerr << "Usage:\n" << "test.exe" << "   arg_1   arg_2   arg_3   arg_4  arg_5  arg_6    arg_7\n\n";
        std::cout << "Required arguments:\n";
        std::cout << "argument_1: n_ped (# pedestrians) = [100, 200], integer\n";
        std::cout << "argument_2: n_injured (# injuries) = 10 (default), integer\n";
        std::cout << "argument_3: n_committed (# committed volunteers) = [0, 10], integer\n";
        std::cout << "argument_4: payoff_T (temptation) = [0, 2]\n";
        std::cout << "argument_5: payoff_S (sucker's payoff) = [-1, 1]\n";
        std::cout << "argument_6: beta (selection pressure)\n";
        std::cout << "argument_7: value_ls (sensory range)\n";
        std::cout << "argument_8: n_iterations (# of runs)\n\n";
        return 1;
    }
    else{
        //
        int     n_ped = atof(argv[1]);          // the number of pedestrians: default = 100
        int     n_injuries = atof(argv[2]);     // the number of injured persons: default = 10
        int     n_committed = atof(argv[3]);    // the number of committed volunteers
        double  payoff_T = atof(argv[4]);       // temptation
        double  payoff_S = atof(argv[5]);       // sucker's payoff
        double  beta = atof(argv[6]);           // selection pressure
        double  value_ls = atof(argv[7]);       // selection pressure
        int     n_iterations = atof(argv[8]);   // the number of iterations
        //
        //std::cout << run_id_begin <<"\t"<< (run_id_begin+n_iterations) <<"\n";
        //
        for (int run_id = run_id_begin; run_id < (run_id_begin+n_iterations); run_id++){
            //
            std::cout <<"n_ped = " << n_ped  <<" ";
            std::cout <<"n_injured = " << n_injuries  <<" ";
            std::cout <<"n_committed = " << n_committed  <<" ";
            std::cout <<"T = "      << payoff_T <<" ";
            std::cout <<"S = "      << payoff_S <<" ";
            std::cout <<"beta = "   << beta <<" ";
            std::cout <<"run_id = " << run_id   <<"\n";
            //
            // open a log file
            std::stringstream filename_log;
            filename_log << "log.txt";
            std::ofstream write_log(filename_log.str().c_str(), std::ios_base::out|std::ios_base::app);
            //
            write_log <<"n_ped = " << n_ped <<"\t";
            write_log <<"n_injured = " << n_injuries <<"\t";
            write_log <<"n_committed = " << n_committed <<"\t";
            write_log <<"T = "      << payoff_T <<"\t";
            write_log <<"S = "      << payoff_S <<"\t";
            write_log <<"beta = "   << beta <<"\t";
            write_log <<"run_id = " << run_id <<"\n";
            //
            // run scenarios
            Scenario RoomEvac(run_id, n_ped, n_injuries, n_committed, payoff_T, payoff_S, beta, value_ls); // initialize RoomEvac
            RoomEvac.runScenario();
            //
        }
        return 0;
    }
    //
}
