#include "system_bg.h"
#include "commonsi.h"

using namespace std;

int system_bg::cat(const char* args) {
    istringstream iss(args);
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(f_paths));
//    cout << f_paths.size() << endl;
    if (f_paths.size() <= 0) {
//        cout << "Reading from stdin..." << endl;
        string line;
        do {
            cin >> line;
            cout << line << endl;
        } while (!cin.eof());
    }
    else {
        for(std::vector<string>::iterator it = f_paths.begin(); it != f_paths.end(); ++it) {
//            cout << "Reading from file " << *it << endl;
            output_file(it->c_str());
        }
    }
    return 0;
}

int system_bg::output_file(const char* path) {
    string line;
    ifstream file_in (path);
    string buff;
    string jobinput_path;
    int retcode = 1;
    if (file_in.is_open())
    {
        while ( getline (file_in, line) )
            cout << line << endl;
        file_in.close();
        retcode = 0;
    }
    else {
        cerr << "cat: " << path << ": " << "No such file or directory" << endl;   
    }
    return retcode;
}

int system_bg::cp(const char* args)
{
    int retcode = 0;
    istringstream iss(args);
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(f_paths));
//    cout << f_paths.size() << endl;
    if (f_paths.size() < 2) {
        if (f_paths.size() < 1)
            cerr << "cp: missing file operand" << endl;
        else
            cerr << "cp: missing destination file operand after `" << args << "'" << endl;
        return 1;
    }

    struct stat dest_stat;
    bool target_is_dir = true;
    int retcode_stat = stat(f_paths[f_paths.size()-1].c_str(), &dest_stat);
    if ( retcode_stat == -1 ) {
        if ( f_paths.size() == 2 && basename(f_paths[f_paths.size()-1]) != "") {
            target_is_dir = false;
        }
        else if ( !S_ISDIR(dest_stat.st_mode) ) {
            cerr  << "cp: target `" << f_paths[f_paths.size()-1] << "' is not a directory" << endl;
            return 1;
        }
    }
    for ( int i = 0; i < f_paths.size() - 1; i++) {
        string source_f = f_paths[i];
        std::ifstream input( source_f.c_str(), std::ios::binary );
        if (!input.is_open()) {
            cerr << "cp: cannot stat `" << source_f << "': No such file or directory" << endl;
            retcode = 1;
        }
        string target_f = f_paths[f_paths.size()-1];
        if ( target_is_dir )
            target_f += "/" + basename(source_f);
//        cout << f_paths[i] << ": " << target_f << endl;
        std::ofstream output( target_f.c_str(), std::ios::binary );
        std::copy( 
            std::istreambuf_iterator<char>(input), 
            std::istreambuf_iterator<char>( ),
            std::ostreambuf_iterator<char>(output));
    }
    return retcode;
}

struct MatchPathSeparator
{  
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};

std::string system_bg::basename( std::string const& pathname )
{
    return std::string( 
            std::find_if( pathname.rbegin(), pathname.rend(),
                MatchPathSeparator() ).base(),
            pathname.end() );
}

int system_bg::mv(const char* args)
{
    int retcode = 0;
    istringstream iss(args);
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(f_paths));
//    cout << f_paths.size() << endl;
    if (f_paths.size() < 2) {
        if (f_paths.size() < 1)
            cerr << "mv: missing file operand" << endl;
        else
            cerr << "mv: missing destination file operand after `" << args << "'" << endl;
        return 1;
    }
    struct stat dest_stat;
    bool target_is_dir = true;
    int retcode_stat = stat(f_paths[f_paths.size()-1].c_str(), &dest_stat);
    if ( retcode_stat == -1 ) {
        if ( f_paths.size() == 2 && basename(f_paths[f_paths.size()-1]) != "") {
            target_is_dir = false;
        }
        else if ( !S_ISDIR(dest_stat.st_mode) ) {
            cerr  << "mv: target `" << f_paths[f_paths.size()-1] << "' is not a directory" << endl;
            return 1;
        }
    }
    for ( int i = 0; i < f_paths.size() - 1; i++) {
        string source_f = f_paths[i];
        std::ifstream input( source_f.c_str(), std::ios::binary );
        if (!input.is_open()) {
            cerr << "mv: cannot stat `" << source_f << "': No such file or directory" << endl;
            retcode = 1;
        }
        string target_f = f_paths[f_paths.size()-1];
        if ( target_is_dir )
            target_f += "/" + basename(source_f);
        cout << source_f.c_str() << " --> " <<  target_f.c_str() << endl;
        rename( source_f.c_str(), target_f.c_str() );
    }
    return retcode;
}

int system_bg::rm(const char* args)
{
    int retcode = 0;
    istringstream iss(args);
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(f_paths));
//    cout << f_paths.size() << endl;
    if (f_paths.size() < 1) {
        cerr << "rm: missing file operand" << endl;
        return 1;
    }

    for ( int i = 0; i < f_paths.size(); i++) {
        if ( unlink( f_paths[i].c_str() ) == -1 ) {
            cerr << "rm: cannot remove `" << f_paths[i] << "': " << strerror( errno ) << endl;
            retcode = 1;
        }
    }
    return retcode;
}

int system_bg::shell_rmdir(const char* args)
{
    int retcode = 0;
    istringstream iss(args);
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(f_paths));
//    cout << f_paths.size() << endl;
    if (f_paths.size() < 1) {
        cerr << "rmdir: missing file operand" << endl;
        return 1;
    }

    for ( int i = 0; i < f_paths.size(); i++) {
        if ( rmdir( f_paths[i].c_str() ) == -1 ) {
            cerr << "rmdir: failed to remove `" << f_paths[i] << "': " << strerror( errno ) << endl;
            retcode = 1;
        }
    }
    return retcode;
}

int system_bg::shell_from_syscall(const char* command, const char* args, int min_args, int max_args, const char* errhead)
{
cout<<"system_bg::shell_from_syscall-I-Running "<<command<<endl;
    int retcode = 0;
    istringstream iss(args);
    std::vector<string> cmd_args;
    std::vector<string> cmd_options;
    std::vector<string> f_paths;
    copy(istream_iterator<string>(iss),
        istream_iterator<string>(),
        back_inserter(cmd_args));
    for ( int i = 0; i < cmd_args.size(); i++) {
        if ( cmd_args[i].find("--") == 0 ) 
            cmd_options.push_back( cmd_args[i].substr( 2 ) );
        else if ( cmd_args[i].find("-") == 0 ) 
            for (int j = 0; j < cmd_args[i].substr( 1 ).length(); j++ )
                 cmd_options.push_back( cmd_args[i].substr( j+1, 1 ) );
        else 
            f_paths.push_back( cmd_args[i] );
    }
    if ( min_args >= 0 && f_paths.size() < min_args ) {
        cerr << command << ": missing file operand" << endl;
        return 1;
    }
    if ( max_args >= 0 && f_paths.size() > max_args ) {
        cerr << command << ": too many operands" << endl;
        return 1;
    }

    for ( int i = 0; i < f_paths.size(); i++) {
        int syscallret = 0;
        if ( strcmp( command, "mkdir" ) == 0) {
            if ( std::find(cmd_options.begin(), cmd_options.end(), "p") != cmd_options.end() )
                syscallret = mkdir_p( f_paths[i].c_str() );
            else
                syscallret = mkdir( f_paths[i].c_str(), S_IRWXU );
        }
        else if ( strcmp( command, "rmdir" ) == 0) { 
            syscallret = rmdir( f_paths[i].c_str() );
        }
        else if ( strcmp( command, "rm" ) == 0) { 
            if ( std::find(cmd_options.begin(), cmd_options.end(), "r") != cmd_options.end() )
                syscallret = nftw( f_paths[i].c_str(), rm_r, OPEN_MAX, FTW_DEPTH );
            else
                syscallret = unlink( f_paths[i].c_str() );
        }
        else if ( strcmp( command, "touch" ) == 0) { 
            syscallret = touch( f_paths[i] );
        }
        if ( syscallret == -1 ) {
            cerr << command << ": " << errhead << " `" << f_paths[i] << "': " << strerror( errno ) << endl;
            retcode = 1;
        }
    }
    return retcode;
}

int system_bg::mkdir_p(const char *dir)
{
    char tmp[PATH_MAX];
    char *p = NULL;
    size_t len;
    snprintf(tmp, sizeof(tmp),"%s",dir);
    len = strlen(tmp);
    if(tmp[len - 1] == '/')
        tmp[len - 1] = 0;
    for(p = tmp + 1; *p; p++)
        if(*p == '/') {
            *p = 0;
            mkdir(tmp, S_IRWXU);
            *p = '/';
        }
    int retcode = mkdir(tmp, S_IRWXU);
    if (retcode == -1 && errno == EEXIST) 
        retcode = 0;
    return retcode;
}

/* Call unlink or rmdir on the path, as appropriate. */
int system_bg::rm_r( const char *path, const struct stat *s, int flag, struct FTW *f )
{
    int status;
    int (*rm_func)( const char * );

    switch( flag ) {
    default:     rm_func = unlink; break;
    case FTW_DP: rm_func = rmdir;
    }
    if( status = rm_func( path ), status != 0 )
        perror( path );
    return status;
}

int system_bg::touch(const std::string& pathname)
{
    int fd = open(pathname.c_str(),
                  O_WRONLY|O_CREAT|O_NOCTTY|O_NONBLOCK,
                  0666);
    if (fd<0) // Couldn't open that path.
    {
        std::cerr
            << __PRETTY_FUNCTION__
            << ": Couldn't open() path \""
            << pathname
            << "\"\n";
        return -1;
    }
    return 0;
}

int system_bg::grep_and_write(string input_file, string str_grep, string grep_args, string output_file)
{
    string line;
    int nMatched = -1;
    bool caseinsensitive = false;
    if ( grep_args.find("i") != std::string::npos ) {
        std::transform(str_grep.begin(), str_grep.end(), str_grep.begin(), ::tolower);
        caseinsensitive = true;
    }
    ifstream infile (input_file.c_str());
    ofstream outfile (output_file.c_str());
    if (infile.is_open() && outfile.is_open()) {
        nMatched = 0;
        while ( getline (infile,line) ) {
            string tocompare = line;
            if (caseinsensitive) {
                std::transform(tocompare.begin(), tocompare.end(), tocompare.begin(), ::tolower);
            }
            if (tocompare.find(str_grep) != std::string::npos) {
                outfile << line << endl;
                nMatched++;
            }
        }
    }
    outfile.close();
    infile.close();
    return nMatched;
}

int system_bg::system(const char* commandline) {
    cout << "system_bg::system-I-SystemOSNameIs: " << AMSCommonsI::getosname() << " "<<commandline<<endl;
    if (strstr(AMSCommonsI::getosname(), "CNK") == NULL && !strstr(commandline,"touch")) 
        return ::system(commandline);
    const char* args;
    args = strstr(commandline, " ") + 1;
    if (strstr(commandline, "cat") == commandline) {
        return cat(args);
    }
    else if (strstr(commandline, "cp ") == commandline) {
        return cp(args);
    }
    else if (strstr(commandline, "mv ") == commandline) {
        return mv(args);
    }
  else if (strstr(commandline, "rm -r") == commandline) {
        return shell_from_syscall( "rmdir", args, 1, -1, "cannot remove");
    }
    else if (strstr(commandline, "rm ") == commandline) {
        return shell_from_syscall( "rm", args, 1, -1, "cannot remove");
    }
    else if (strstr(commandline, "rmdir ") == commandline) {
        return shell_from_syscall( "rmdir", args, 1, -1, "failed to remove");
    }
    else if (strstr(commandline, "mkdir ") == commandline) {
        return shell_from_syscall( "mkdir", args, 1, -1, "cannot create directory");
    }
    else if (strstr(commandline, "touch ") == commandline) {
        return shell_from_syscall( "touch", args, 1, -1, "cannot touch");
    }
    return ::system(commandline);
}
