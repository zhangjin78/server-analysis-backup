#include <sstream>
#include <iostream>
#include <vector>
#include <occi.h>
#include <stdlib.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
//        elems.push_back(item);
        elems.insert(elems.begin(),item);
    }
    return elems;
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

int file_exists(std::string& path, int stage = 0);

int set_stage_env(void) {
    char* stage_host  = getenv("STAGE_HOST");
    char* stage_class = getenv("STAGE_SVCCLASS");
    if (stage_class == NULL) {
        std::string default_stage_class = "amsuser";
        if (strcmp(getlogin(), "ams") == 0) {
            default_stage_class = "amscdr";
        }
        std::cout << "eos_search-W-STAGE_CLASS-not-set: try " << default_stage_class << std::endl;
        setenv("STAGE_SVCCLASS", default_stage_class.c_str(), 1);
    }
}

int set_oracle_env(void) {
    char* ora_home = getenv("ORACLE_HOME");
    if (ora_home == NULL) {
        struct utsname uts;
        uname(&uts);
        std::string el_ver = uts.release;
        std::string ora_ver;
        if (el_ver.find("el5") != std::string::npos || el_ver.find("el6") != std::string::npos) {
            ora_ver = "prod";
        }
        else if (el_ver.find("el7") != std::string::npos) {
            ora_ver = "19300";
        }
        else {
            std::cerr << "eos_search-F-ORACLE_HOME-not-set" << std::endl;
            return -2;
        }

        std::string default_ora_home = "/afs/cern.ch/project/oracle/@sys/" + ora_ver;
        // std::cerr << "eos_search-W-ORACLE_HOME-not-set: try " << default_ora_home << std::endl;
        setenv("ORACLE_HOME", default_ora_home.c_str(), 1);
        ora_home = getenv("ORACLE_HOME");
        // std::cout << "ORACLE_HOME is : " << ora_home << default_ora_home << std::endl;
    }
    struct stat info;
    if (ora_home == NULL) {
        std::cerr << "eos_search-F-ORACLE_HOME-not-set" << std::endl;
        return -2;
    }
    else if (stat( ora_home, &info ) != 0) {
        std::cerr << "eos_search-F-ORACLE_HOME-not-found: <" << ora_home << ">" << std::endl;
        return -2;
    }
    return 0;
}

int eos_search(const std::string & predicate, std::string & output, int stage, int search_by_time = 0) {
    if (set_oracle_env() != 0) {
        return -2;
    }
    const std::string EOSPREFIX = "root://eosams.cern.ch///eos/ams";
    const std::string CTAPREFIX = "root://eosctapublic.cern.ch//eos/ctapublic/archive/ams";
    oracle::occi::Environment* environment;
    oracle::occi::Connection *con;
    oracle::occi::Statement* stmt;
    oracle::occi::ResultSet* res;

    std::vector<std::string> preds = split(predicate, ':');
    std::string datamc = "";
    std::string temp_name = "";
    std::string runs = "";
    int nPreds = 0;
    while (preds.size()) {
        std::string buf;
        buf = preds.back();
        if (buf.compare("data") == 0) buf = "Data";
        if (buf.compare("mc") == 0) buf = "MC";
        if (buf.compare("Data") == 0 || buf.compare("MC") == 0) {
            datamc = buf;
        }
        else if (!isdigit((buf.c_str())[0]) && buf.find(",") == std::string::npos) {
            temp_name = buf;
        }
        else {
            runs = buf;
        }
        nPreds++;
        preds.pop_back();
    }
    if (nPreds <= 0) {
        return -1;
    }
    if (datamc.empty()) datamc = "Data";
    if (temp_name.empty()) temp_name = "%";
    if (runs.find_last_of(",") == runs.length()-1) runs = runs.substr(0, runs.length()-1);
    int nFound = 0;
    try{
        environment = oracle::occi::Environment::createEnvironment(oracle::occi::Environment::DEFAULT);
        con = environment->createConnection("amsro", "eto_amsro", "pdb_ams");
        std::string pathexpr = "%/" + datamc + "/%/" + temp_name + "/%";
        std::string runexpr = "";
        std::string runselect = "";
        std::string cur_time = "";
        if (search_by_time != 0) {
            runselect = "(";
            while (runs.find_first_of(",") !=  std::string::npos) {
                cur_time = runs.substr(0, runs.find_first_of(","));
                runs = runs.substr(runs.find_first_of(",") + 1);
                runselect += " fetime <= " + cur_time + " and letime >= " + cur_time + " or ";
            }
            runselect += " fetime <= " + runs + " and letime >= " + runs + ") ";
            runexpr = " and run in (select run from amsdes.datafiles where type like 'SCI%%' and " + runselect + ")";
        }
        else if (!runs.empty()) {
            runexpr =  " and run in (" + runs + ") ";
        }
        
        std::string sql = "select path,eostime,ctatime,run from amsdes.ntuples where path like '" + pathexpr + "' " + runexpr + " and (eostime > 0 or ctatime > 0)";
        stmt = con->createStatement(sql);
        res = stmt->executeQuery();
        
        while (res->next()) {
            std::string path = res->getString(1);
            unsigned long eostime = res->getInt(2);
            unsigned long  ctatime = res->getInt(3);
            unsigned long  run_num = res->getInt(4);
            std::size_t pos = path.find("/Data/");
            if (pos == std::string::npos) {
                pos = path.find("/MC/");
            }
            if (pos == std::string::npos) {
                continue;
            }
            else {
                if (eostime > 0) {
                    path = EOSPREFIX + path.substr (pos);
                }
                else if (ctatime > 0) {
                    path = CTAPREFIX + path.substr (pos);
                }
            }
            if (file_exists(path, stage) == 0) {
                output +=  path + ",";
                if (search_by_time != 0) {
                    output += patch::to_string(run_num) + ",";
                }
                nFound++;
            }
        }
        stmt->closeResultSet(res);
        con->terminateStatement(stmt);
        environment->terminateConnection(con);

    }
    catch(oracle::occi::SQLException &e){
        std::cerr << "eos_search-F-SQLException: " << e.what() << std::endl;
        return -1;
    }
    return nFound;
}

int file_exists(std::string& path, int stage) {
    const std::string CTALS = "xrdfs root://eosctapublic.cern.ch ls ";
    const std::string REDIR = " >/dev/null 2>&1 ";
    std::size_t pos_eos = path.find("/eos/ams/");
    std::size_t pos_cta = path.find("/eos/ctapublic/archive/ams/");
    std::size_t pos = std::string::npos;
    std::string cmd;
    if (pos_eos != std::string::npos) {
        pos = pos_eos;
        cmd = "eos ls " + path.substr(pos) + REDIR + " || ls " + path.substr(pos) + REDIR + " || xrdfs root://eosams.cern.ch ls " + path.substr(pos) + REDIR;
    }
    else if (pos_cta != std::string::npos) {
        pos = pos_cta;
        cmd = CTALS + path.substr(pos) + REDIR;
    }
    else {
        std::cerr << "Path does not look like an EOS or CTA path: " << path << std::endl;
        return -1;
    }
    int ret_ls = system(cmd.c_str()) >> 8;
    if (ret_ls) {
        std::cerr << "eos_search-E-file-not-found: \"" << cmd << "\" returned: " << ret_ls << std::endl;
        system("eos --version");
        system("env");
    }
    else if (stage && pos_cta != std::string::npos) {
        set_stage_env();
        std::string cmd_stage = "xrdfs root://eosctapublic.cern.ch prepare -s " + path.substr(pos);
        int ret_stage = system(cmd_stage.c_str());
        std::cout << "eos_search-I-stager-file: \"" << cmd_stage << "\" returned: " << ret_stage << std::endl;
    }
    return ret_ls;
}
