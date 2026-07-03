#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <string.h>
#include <errno.h>
#include <locale.h>
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/utsname.h>
#include "signal.h"
#include <math.h>
#ifdef JUQUEEN
#include <spi/include/kernel/memory.h>
#endif

#include "MPIEmulator.h"
#include "system_bg.h"
#include "commonsi.h"
extern "C"  void lopen_(int &lun, char *fname, int len);
extern "C" void lclose_(int& lun);
using namespace std;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

MPIEmulator::MPIEmulator() {lun = 5;nJobs=0;Memsizekb=0;Signalled=0;RunMode=0;time(&WallClockInit);WallClockTermination=300;WallClockLimit=0;}

int MPIEmulator::InitTime(){
    time_t x;
    time(&x);
    return x-WallClockInit-iRank/8-8*(sqrt(getTotal())-1);
}
    
int MPIEmulator::Terminate(bool hard){
    time_t x;
    time(&x);
    static int i=0;
    if(i++%15==0)cout <<"  MPIEmulator::Terminate()-I- "<<x-WallClockInit<<" "<<WallClockLimit<<" "<<WallClockTermination<<" "<<endl;
    return hard?(x-(WallClockLimit-120+WallClockInit)):(x-(WallClockLimit-WallClockTermination+WallClockInit)); 
} 
int MPIEmulator::MPI_Init(int& argc, char** argv)
{
    time(&WallClockInit);
    cout << "MPIEmulator::MPI_Init-I-Starting " << WallClockInit<<endl;
    struct utsname uts;
    uname(&uts);
    cout << "MPIEmulator::MPI_Init-I-uts.machine : " << uts.machine << endl;
    cout << "MPIEmulator::MPI_Init-I-uts.sysname : " << uts.sysname << endl;
    sSystem = string(uts.sysname);
    mpi_status = 1;
    char* mpi_jid_list = ::getCmdOption(argv, argv+argc, "--mpijidlist");
    char* ll_jobname = ::getCmdOption(argv, argv+argc, "--llname");
    char* total_tasks = ::getCmdOption(argv, argv+argc, "--totaltasks");
    char *wall_time  = ::getCmdOption(argv, argv+argc, "--walltime");
if(wall_time){
    char *pch = strtok (wall_time," :");
      vector<int> v;
      while(pch!=NULL){
       v.push_back(atol(pch));
       pch = strtok (NULL, " :");
      }
            const int sec[4]={1,60,3600,86400};
     if(v.size()>4 ){
      WallClockLimit=1000000000 ;
      cerr<<"MPIEmulator::MPI_Init-W-UnableToSetWallClock "<<endl;
     }
     else{
        for(int l=0;l<v.size();l++){
         WallClockLimit+=v[v.size()-l-1]*sec[l];
      }
     }
     cout<<"MPIEmulator::MPI_Init-I-WallClockCalculatedAs "<<WallClockLimit<<endl;;
     if(WallClockLimit>1800)WallClockTermination+=(WallClockLimit-1800)/30;
     else if (WallClockLimit<600) {
          cerr<<"MPIEmulator::MPI_Init-F-WallClockLimitTooSmall "<<WallClockLimit<<endl;
          exit(1);
          } 
}
else {
WallClockLimit=1000000000 ;
if(total_tasks){
cerr<<"MPIEmulator::MPI_Init-F-UnableToSetWallClock "<<endl;
exit(1);
}
}
#ifdef JUQUEEN 
   uint64_t heapavail,heap;
   Kernel_GetMemorySize(KERNEL_MEMSIZE_HEAPAVAIL, &heapavail);
   Kernel_GetMemorySize(KERNEL_MEMSIZE_HEAP, &heap);
   Memsizekb=heap/1024+heapavail/1024;
   if(Memsizekb<1024*1024){
      cerr<<"MPIEmulator::MPI_Init-E-MemSizeProblemSettinhto16Gb "<<Memsizekb<<endl;
      Memsizekb=16*1024*1024;
   }
   else cout<<"MPIEmulator::MPI_Init-MemsizeSetTo "<<Memsizekb<< " KB "<<endl;
#endif
      if (mpi_jid_list != NULL && ll_jobname != NULL && total_tasks != NULL) {
        cout << "MPIEmulator::MPI_Init-I-Argument: --mpijidlist " << mpi_jid_list << endl;
        cout << "MPIEmulator::MPI_Init-I-Argument: --llname " << ll_jobname << endl;
        cout << "MPIEmulator::MPI_Init-I-Argument: --totaltasks " << total_tasks << endl;
    }
    else if (mpi_jid_list == NULL && ll_jobname == NULL && total_tasks == NULL) {
        iJID = 0;
        nJobs = 0;
        cout << "MPIEmulator::MPI_Init-I-NormalJobParameters: No MPI specific parameter detected, continue as normal job." << endl;
        mpi_status=0;
        return 0;
    }
    else {
        mpi_status = -1;
        cerr << "MPIEmulator::MPI_Init-F-IncorrectParameters: Please specify --mpijidlist <jid_list> --llname <job_name> --totaltasks <num_jobs>" << endl;
        return mpi_status;
    }
    const char* env_amsprodlogdir = std::getenv("AMSProductionLogDir");
    if (env_amsprodlogdir == NULL) {
        cerr << "MPIEmulator::MPI_Init-F-AMSProductionLogDirUndefined: Cannot get environment AMSProductionLogDir." << endl;
        mpi_status = -1;
        return mpi_status;
    }
    prod_log_dir = string(env_amsprodlogdir);
    job_working_dir = prod_log_dir + "/" + string(ll_jobname);
    string cmd_create_job_working_dir = "mkdir -p " + job_working_dir;
    if (system_bg::system(cmd_create_job_working_dir.c_str()) != 0) {
        cerr << "MPIEmulator::MPI_Init-F-CannotCreateDirectory: " << cmd_create_job_working_dir << " failed - " << strerror( errno ) << endl;
        mpi_status = -1;
        return mpi_status;
    }
    exit_flag_path = job_working_dir + "/finished";
    char * pend;
    nJobs = strtol(total_tasks, &pend, 10);
    iRank = getMyMPIRank();
       WallClockInit-=iRank/8+8*(sqrt(getTotal())-1);
    if (iRank > nJobs - 1) {
        mpi_status = -2;
        cerr << "MPIEmulator::MPI_Init-F-JobIDListLargerMaxID: " << iJID << " > " << nJobs << "-1" << endl;
        MPI_Finalize();
        return mpi_status;
    }
    else if (iRank >= 0) {
        cout << "MPIEmulator::MPI_Init-I-MPIRankSetTo: " << iRank << endl;
    }
    else {
        mpi_status = -1;
        cerr << "MPIEmulator::MPI_Init-F-CannotGetMPIRank: quitting..." << endl;
        return mpi_status;
    }
    iJID = getJobID(mpi_jid_list);
    if (iJID == 0) {
        mpi_status = -1;
        cerr << "MPIEmulator::MPI_Init-F-IncorrectJobIDList: " << mpi_jid_list << endl;
        return mpi_status;
    }
    else if (iJID < 0) {
        mpi_status = -1;
        cerr << "MPIEmulator::MPI_Init-F-TotalTasksNumberNotMatchJobIDList: " << nJobs << " vs " << mpi_jid_list << endl;
        return mpi_status;
    }
    mpi_status = 1;
    cout << "MPIEmulator::MPI_Init-I-JIDSetTo: " << iJID << endl;
    stringstream ss;
    ss << iJID;
    string jid = ss.str();
    job_log_f_path = prod_log_dir + "/" + jid + ".log";
    string cmd_cp_tmp_log = "cp " + string(AMSCommonsI::gettmplogpath()) + " " + job_log_f_path;
    char *lr=getenv("LogRedirection");
    if (lr && strlen(lr) &&strstr(lr,"commonsi.C") ) {
        system_bg::system(cmd_cp_tmp_log.c_str());
        freopen(job_log_f_path.c_str(), "a", stdout);
        system_bg::system(cmd_cp_tmp_log.c_str());
        int fd[2];
        pid_t pid;
        pipe(fd);
        pid=fork();
        if(pid==0) {
            dup2(fd[0], fileno(stdin));
            close(fd[1]);
            close(fd[0]);
            execlp("gawk", "gawk", "{ print strftime(\"[%Y-%m-%d %H:%M:%S]\"), $0 }", (char*) NULL);
            exit(1);
        }
        else {
            dup2(fd[1], fileno(stdout));
            dup2(fd[1], fileno(stderr));
            close(fd[0]);
            close(fd[1]);
        }
        cout << "MPIEmulator::MPI_Init-I-LogFileMovedTo: " << job_log_f_path << endl;
        unlink(AMSCommonsI::gettmplogpath());
        cout << "MPIEmulator::MPI_Init-I-StartingLog: " << iJID << " in file " << job_log_f_path << endl;

    }
    else if(lr){
        cout << "MPIEmulator::MPI_Init-I-StartingLog: " << iJID << " in file " << job_log_f_path << endl;
        freopen(job_log_f_path.c_str(), "w", stdout);
        int fd[2];
        pid_t pid;
        pipe(fd);
        pid=fork();
        if(pid==0) {
            dup2(fd[0], fileno(stdin));
            close(fd[1]);
            close(fd[0]);
            execlp("gawk", "gawk", "{ print strftime(\"[%Y-%m-%d %H:%M:%S]\"), $0 }", (char*) NULL);
            exit(1);
        }
        else {
            dup2(fd[1], fileno(stdout));
            dup2(fd[1], fileno(stderr));
            close(fd[0]);
            close(fd[1]);
        }
        cout << "MPIEmulator::MPI_Init-I-StartingLog: " << iJID << " in file " << job_log_f_path << endl;
    }
    cout << "MPIEmulator::MPI_Init-I-LocatingJobScript: Starting job " << jid << " from " << prod_log_dir << endl;
    cout << "MPIEmulator::MPI_Init-I-starting " << WallClockInit<<endl;
    // Reading job file $AMSProductionLogDir/<JID>.job
    job_path = prod_log_dir + "/" + jid + ".job";
    string line;
    ifstream jobfile (job_path.c_str());
    string job_input_buff;
    vector<string> lines;
    if (jobfile.is_open())
    {
        cout << "MPIEmulator::MPI_Init-I-JobScriptFound: " << job_path << endl;
        bool start_input = 0;
        while ( getline (jobfile,line) ) {
            if (start_input) {
                if (line.find("!" != 0)) {
                    istringstream iss(line);
                    std::vector<string> tokens;
                    copy(istream_iterator<string>(iss),
                            istream_iterator<string>(),
                            back_inserter(tokens));
                    for(std::vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
                        int start = 0;
                        size_t startpos = it->find("$", start);
                        int i;
                        while (startpos != std::string::npos) {
                            for (i = startpos+1; i < it->length(); i++)
                                if (!std::isalnum((*it)[i]) && (*it)[i] != '_')
                                    break;
                            string varname = it->substr(startpos+1, i-startpos-1);
                            job_input_buff += it->substr(start, startpos-start);
                            job_input_buff += parseVariable(lines, varname);
                            start = i;
                            startpos = it->find("$", start);
                        }
                        if (start < it->length())
                            job_input_buff += it->substr(start);
                        job_input_buff += " ";

                    }
                    job_input_buff += "\n";
                }
                else
                    start_input = 0;
            }
            else if (line.find("$ExeDir") == 0) {
                istringstream iss(line);
                std::vector<string> tokens;
                copy(istream_iterator<string>(iss),
                        istream_iterator<string>(),
                        back_inserter(tokens));
                string exec_path;
                for(std::vector<string>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
                    if (it->find("$ExeDir") == 0) {
                        exec_path = *it;
                        argc = 1;
                    }
                    else if (it->find("<<!") == 0) {
                        start_input = 1;
                    }
                    else {
                        argv[argc] = new char[it->length()+1];
                        strcpy(argv[argc], it->c_str());
                        argc++;
                    }
                }
            }
            else {
                lines.push_back(line);
                if (line.find("export ") == 0) {
                    istringstream iss(line);
                    std::vector<string> tokens;copy(istream_iterator<string>(iss),
                            istream_iterator<string>(),
                            back_inserter(tokens));
                    std::vector<string>::iterator it = tokens.begin();
                    it++;
                    if (it != tokens.end()) {
                        int pos_equal = it->find("=");
                        if (pos_equal > 0) {
                            string env_name = it->substr(0, pos_equal);
                            string env_val  = it->substr(pos_equal+1);
                            setenv(env_name.c_str(), env_val.c_str(), 0);
                        }
                    }
                }
            }
        }
        cout << "MPIEmulator::MPI_Init-I-GbatchAargsParsed: " << argc << " arguments:";
        for (int i = 0; i < argc; i++)
            cout << " " << argv[i];
        cout << endl;
        job_input_path = job_working_dir + "/" + jid + ".ftin5";
        cout << "MPIEmulator::MPI_Init-I-WritingDataCards: Writing the following to file " << job_input_path << " :" << endl;
        ofstream jobinput (job_input_path.c_str());
        jobinput << job_input_buff << endl;
        cout << job_input_buff << endl;
        jobinput.close();
        jobfile.close();
        cout << "MPIEmulator::MPI_Init-I-DataCardsWritten: Data cards written to file " << job_input_path << " ." << endl;
        char j_path[1024];
        strcpy(j_path, job_input_path.c_str());
        j_path[strlen(j_path)] = '\0';
        cout << "MPIEmulator::MPI_Init-I-lopenStart: Calling lopen_(" << lun << ", " << j_path << ", " << strlen(j_path)+1 << ") ..." << endl;
        lopen_(lun,j_path,strlen(j_path)+1);
        cout << "MPIEmulator::MPI_Init-I-InitializationCompleted." << endl;
    }
    else {
        cerr << "MPIEmulator::MPI_Init-F-CannotOpenJobScript: " << job_path << " - " << strerror( errno ) << endl;
        mpi_status = -2;
        MPI_Finalize();
        return mpi_status;
    }
    return 0;
}

int MPIEmulator::YetAnotherFinished()
{
static string   finish_flag_path = job_working_dir + "/finished";
    int fd = open(finish_flag_path.c_str(), O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        cerr << "MPIEmulator::YetAnotherFinished-S-CannotOpenFile: " << finish_flag_path << " - " << strerror( errno ) << endl;
        close(fd); 
        return -1;
    }
    int result = lockf(fd, F_LOCK, 0);
    if (result != 0) {
        cerr << "MPIEmulator::YetAnotherFinished-S-CannotLockFile: " << finish_flag_path << " - " << strerror( errno ) << endl;
        close(fd); 
        return -2;
    }
    int i = 0;
    char buf[13];
    if (read(fd, &buf[0], 9) == 0)i = 0;
    else i = atol(buf);
    sprintf(buf, "%d", ++i);
    lseek(fd, 0, SEEK_SET);
    write(fd, &buf[0], strlen(buf));
    result = lockf(fd, F_ULOCK, 0);
    if (result != 0) {
        cerr << "MPIEmulator::YetAnotherFinished-S-CannotUnlockFile: " << finish_flag_path << " - " << strerror( errno ) << endl;
        close(fd); 
        return -3;
    }
    close(fd); 
    return i;
}
long MPIEmulator::getMyMPIRank()
{
    start_flag_path = job_working_dir + "/started";
    int fd = open(start_flag_path.c_str(), O_CREAT | O_RDWR, 0600);
    if (fd < 0) {
        cerr << "MPIEmulator::getMyMPIRank-F-CannotOpenLockFile: " << start_flag_path << " - " << strerror( errno ) << endl;
        return -1;
    }
    int result = lockf(fd, F_LOCK, 0);
    if (result != 0) {
        cerr << "MPIEmulator::getMyMPIRank-F-CannotLockFile: " << start_flag_path << " - " << strerror( errno ) << endl;
        return -1;
    }
    int i = 0;
    char buf[20];
    if (read(fd, &buf[0], 9) == 0)
        i = 0;
    else {
        i = atoi(buf);
        i++;
    }
    cout << "MPIEmulator::getMyMPIRank-I-Read :"<<i-1<<endl;
    sprintf(buf, "%d", i);
    lseek(fd, 0, SEEK_SET);
    write(fd, &buf[0], strlen(buf));
    result = lockf(fd, F_ULOCK, 0);
    if (result != 0) {
        cerr << "MPIEmulator::getMyMPIRank-F-CannotUnlockFile: " << start_flag_path << " - " << strerror( errno ) << endl;
        return -1;
    }
    close(fd); 
    return i;
}

int MPIEmulator::getFinished(){
    //  Get number of finished jobs
    if(mpi_status <= 0 && mpi_status >= -1)return -1;
   int iFinished=-1;
    static string finish_flag_path = job_working_dir + "/finished";
    int fd = open(finish_flag_path.c_str(), O_RDONLY, 0600);
    if (fd < 0) {
       close(fd);
        return 0;
    }
    char buf[10];
    if (read(fd, &buf[0], 9) == 0)iFinished = 0;
    else iFinished = atol(buf);
    close(fd);
    return iFinished;
}
bool MPIEmulator::AnyOneFinished(){
    static bool yes=false;
     if(yes)return yes;
    if(mpi_status <= 0 && mpi_status >= -1)return yes;
    int iFinished=-1;
    static string finish_flag_path = job_working_dir + "/finished";
    int fd = open(finish_flag_path.c_str(), O_RDONLY, 0600);
    if (fd < 0) {
       close(fd);
        return yes;
    }
    char buf[10];
  if(read(fd, &buf[0], 9) == 0)return yes;
  else iFinished = atol(buf);
   close(fd);
   if(iFinished)yes=true; 
   return yes;
}
int  MPIEmulator::bcast(bool set){
     static bool bset=false;
      if(mpi_status <= 0 && mpi_status >= -1)return 0;
    char tmp[2048];
    sprintf(tmp,"%s_bcast",exit_flag_path.c_str());
     if(set){
         if(bset)return 0;
         bset=true;
         string touch="touch ";
         touch+=tmp;
         int ret=system_bg::system(touch.c_str());
         if(ret)cerr<<"MPIEmulator::bcast-E-UnableToDo "<<touch<<endl;
                 return ret;
     }
     else{
     struct stat64 buffer;
      if( stat64 (tmp, &buffer ) ==0 )return 1;
      else return 0; 
     } 
} 
void MPIEmulator::MPI_Finalize(void)
{
static bool finalized=false;
if(finalized){
cout << "MPIEmulator::MPI_Finalize-I-FinalizedTrueReturn "<<endl; 
  return;
}
    finalized=true;
    if (mpi_status <= 0 && mpi_status >= -1) {
        cout << "MPIEmulator::MPI_Finalize-I-DoNothingForMPIStatus: " << mpi_status << endl;
#ifndef JUQUEEN
   sleep(2);
    signal(SIGABRT,SIG_DFL);
    signal(SIGSEGV,SIG_DFL);
#endif
        return;
    }
    YetAnotherFinished();
    int ifin=0;   
    while (true) {
            int i=getFinished();
        if(i!=ifin)cout << "MPIEmulator::MPI_Finalize-I-FinalizingFromRankNo: " << iRank << " , " << i << " ranks already finalized." << endl;
            if (i== getTotal() || (i==0 && ifin)) {
                unlink(start_flag_path.c_str());
                lclose_(lun);
                //FIXME
                string remove_work_dir = "rm -r " + job_working_dir;
                system_bg::system(remove_work_dir.c_str());
                cout << "MPIEmulator::MPI_Finalize-I-Finalized: All the " << nJobs << " jobs finished, let's stop." << endl;
                break;
         }
             if(i)ifin=i; 
#ifdef JUQUEEN
         //cout << "MPIEmulator::MPI_Finalize-I-Sleep 5 "<<endl;
            sleep(10);
#else
         break;
#endif
    }
    signal(SIGABRT,SIG_DFL);
    signal(SIGSEGV,SIG_DFL);
    sleep(5); 
    cout << "MPIEmulator::MPI_Finalize-I-ReturnToDestroy "<<endl;
    return;
}

long MPIEmulator::getJobID(char* jid_list) {
    int j = 0;
    long myID = 0;
    char *str1, *str2, *saveptr1, *saveptr2, *token, *start_jid_str, *end_jid_str, *pend;
    
    for (j = 0, str1 = jid_list; ; j++, str1 = NULL) {
        long start_jid, end_jid;
        token = strtok_r(str1, ",", &saveptr1);
        if (token == NULL) {
            break;
        }
        start_jid = strtol(token, &pend, 10);
        end_jid = start_jid;

        str2 = token;
        start_jid_str = strtok_r(str2, "-", &saveptr2);
        end_jid_str = strtok_r(NULL, "-", &saveptr2);
        if (start_jid_str != NULL && end_jid_str != NULL) {
            start_jid = strtol(start_jid_str, &pend, 10);
            end_jid = strtol(end_jid_str, &pend, 10);
            if (start_jid > end_jid) {
                int itmp = end_jid;
                end_jid = start_jid;
                start_jid = itmp;
            }
        }
        if (start_jid <= 0 ||  end_jid <= 0)
            return 0;
        if ( iRank >= j && iRank <= j + (end_jid - start_jid) ) {
            myID = start_jid + (iRank -j);
        }
        j += (end_jid - start_jid);
    }
    if (j == nJobs)
        return myID;
    else
        return -1;
}

string MPIEmulator::parseVariable(vector<string> lines, string name)
{
    string value;
    bool found = false;
    if (name.find("$") == 0)
        name.erase(0, 1); //Getting rid of the leading $
    char* str_value = std::getenv(name.c_str());
    if (str_value) {
        cout << "MPIEmulator::parseVariable-I-FoundInEnvironment: " << name << " = " << str_value << endl;
        value = string(str_value);
        found = true;
    }
    for(std::vector<string>::iterator it = lines.begin(); it != lines.end(); ++it) {
        if (it->find(name+"=") == 0) {
            value = it->substr(name.length()+1);
            found = true;
        }
    }
    if (!found) {
        cerr << "MPIEmulator::parseVariable-W-CannotFindVariable: " << name << " returning empty string." << endl;
        return string("");
    }
    else if (value.empty()) {
        cerr << "MPIEmulator::parseVariable-W-EmptyValuedVariable: " << name << endl;
        return string("");
    }
    else {
        if (value.find("'") == 0)
            value.erase(0, 1);
        if (value.find("'") == value.length() - 1)
            value.erase(value.length() - 1, 1);
        return value;
    }
}

