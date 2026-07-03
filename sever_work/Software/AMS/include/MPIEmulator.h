#include <iostream>
#include <vector>

using namespace std;

class MPIEmulator
{
    public:
        long      nJobs;
        long      iJID;
        int       iRank;
        string    job_working_dir;
        string    start_flag_path;
        string    exit_flag_path;
        string    job_input_buff;
        string    job_input_path;
        string    job_path;
        string    job_log_f_path;
        string    prod_log_dir;
        int       mpi_status;
        long      getMyMPIRank(void);
        int       YetAnotherFinished();
        bool      AnyOneFinished();
        long      getJobID(char*);
        int       lun;
        string    sSystem;
    public:
        int       Memsizekb;
        int       RunMode; 
        int Signalled;
        string    parseVariable(vector<string> lines, string name);
        time_t WallClockInit;
        unsigned int WallClockLimit; 
        unsigned int WallClockTermination;
    public:
        MPIEmulator();
        int  MPI_Init(int& argc, char** argv);
        void MPI_Finalize(void);
        int getFinished();
        int bcast(bool set=false);
        int getRank()const {return iRank;}
        int getTotal()const {return nJobs;}
        int Terminate(bool hard=false);
        int InitTime();
        int  getMPIStatus(void) {return mpi_status;};
        const char* getSystem(void) {return sSystem.c_str();}
        const char* getLogPath(void) {return job_log_f_path.c_str();};
};
