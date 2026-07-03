#include <iostream>


#include <stdlib.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string.h>
#include <unistd.h>

using namespace std;

int eos_search(const string & predicate, string & output, int stage = 0, int search_by_time = 0);

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " [Option(s)] [<data|mc>:[template]:time]\n"
              << "Options:\n"
              << "\t-h,--help\t\tShow this help message.\n"
              << "\t-s,--stage\t \tSend staging command for a tape file.\n"
              << std::endl;
}

int main(int argc, char* argv[]){
    string ntuples;
    string input  = "data:pass6:1341756848";;
    int stage = 0;
    int verbose = 0;
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-h" || arg == "--help") {
            show_usage(argv[0]);
            return 0;
        }
        else if (arg == "-s" || arg == "--stage") {
            stage = 1;
        }
        else if (arg == "-v" || arg == "--verbose") {
            verbose = 1;
        }
        else {
            input = arg;
        }
    }

    int n = eos_search(input, ntuples, stage, 1);
    if (n < 0) {
        cerr << "eos_search returned error code: " << n << endl;
        return n;
    }

    if (verbose > 0) {
        cout << n << " entrie(s) found:" << endl;
    }
    cout << ntuples << endl;
    return 0;
}
