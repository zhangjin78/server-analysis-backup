//#include <mpi.h>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#include <limits.h>
#include <ftw.h>
#include <map>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>

#ifndef MAX_ARG_LENGTH
#define MAX_ARG_LENGTH 1024
#endif

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

using namespace std;

namespace system_bg{
    int cat(const char* args);
    int output_file(const char* path);
    int cp(const char* args);
    int mv(const char* args);
    int rm(const char* args);
    int shell_rmdir(const char* args);
    int mkdir_p(const char *);
    int rm_r( const char *path, const struct stat *s, int flag, struct FTW *f );
    int touch(const std::string& pathname);
    int grep_and_write(string input_file, string str_grep, string grep_args, string output_file);
    int shell_from_syscall(const char* command, const char* args, int min_args, int max_args, const char* errhead);
    std::string basename( std::string const& pathname );
    int system(const char* commandline);
}

