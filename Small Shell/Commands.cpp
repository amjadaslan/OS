#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <sys/wait.h>
#include <iomanip>
#include "Commands.h"
#include <algorithm>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
using namespace std;
#define BUFFER_SIZE 1000

const std::string
        WHITESPACE = " \n\r\t\f\v";
using std::vector;

#if 0
#define FUNC_ENTRY()  \
  cerr << __PRETTY_FUNCTION__ << " --> " << endl;

#define FUNC_EXIT()  \
  cerr << __PRETTY_FUNCTION__ << " <-- " << endl;
#else
#define FUNC_ENTRY()
#define FUNC_EXIT()
#endif

#define DEBUG_PRINT cerr << "DEBUG: "

#define EXEC(path, arg) \
  execvp((path), (arg));HITESPACE

string _ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}

bool _isBackgroundComamnd(const char* cmd_line) {
    const string str(cmd_line);
    return str[str.find_last_not_of(WHITESPACE)] == '&';
}

void _removeBackgroundSign(char* cmd_line) {
    const string str(cmd_line);
    if (str.size() == 0)
        return;
    // find last character other than spaces
    unsigned int idx = str.find_last_not_of(WHITESPACE);
    // if all characters are spaces then return
    if (idx == string::npos) {
        return;
    }
    // if the command line does not end with & then return
    if (cmd_line[idx] != '&') {
        return;
    }
    // replace the & (background sign) with space and then remove all tailing spaces.
    cmd_line[idx] = 0;
    // truncate the command line string up to the last non-space character
}


Command::Command(const char* cmd_line) : cmd_line(cmd_line) {}
string& Command::get_cmd_line() {
    return cmd_line;
}

BuiltInCommand::BuiltInCommand(const char* cmd_line) : Command(cmd_line) {}

string SmallShell::prompt = "smash> ";

string SmallShell::prev_dir = "";

bool SmallShell::exit_flag_quit = false;

bool SmallShell::exit_flag_redirect = false;

bool SmallShell::exit_flag_pipe = false;

bool SmallShell::exit_flag_time_out = false;

JobsList SmallShell::jobs_list;

vector<TimeOutNode> SmallShell::time_out_vec;

bool SmallShell::set_grp = true;

pid_t SmallShell::smash_pid = getpid();

pid_t SmallShell::smash_fg_son = 0;

string SmallShell::cmd_line_signal = "";

SmallShell::SmallShell() {}

SmallShell::~SmallShell() {
// TODO: add your implementation
}


int split(const char* cmd_line, std::vector<std::string>& vec, char delim = ' ')
{
    int i=0;
    std::string str(cmd_line);
    std::string token;
    std::stringstream ss;
    do {
        str = _ltrim(str);
        if(str == "")
            break;
        std::stringstream ss(str);
        std::getline(ss, token, delim);
        vec.push_back(token);
        str.erase(0, token.length());
        i++;
    }
    while (1);
    return i;
}

static void auxi2(void)
{
   vector<int> A;
   for(int i=0;i<A.size();i++)
   {
       A.push_back(i);
   }

    if(A.size()==0)
    {
        return;
    }

    int maxsum=-1000000,currsum=0;
    for(int i=0;i<A.size();i++)
    {
        currsum+=A[i];
        if(currsum>maxsum)
        {
            maxsum=currsum;
        }
        if(currsum<0)
        {
            currsum=0;
        }
    }
    return;
}
/**
* Creates and returns a pointer to Command class which matches the given command line (cmd_line)
*/
Command* SmallShell::CreateCommand(const char* cmd_line) {
    //
    vector<string> args_built_in;
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    string cmd_line_cpy(cmd_line);
    _removeBackgroundSign((char*)cmd_line);
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    int num_args1 = split(cmd_line, args_built_in);
    string cmd_s(args_built_in[0]);
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();

    //TimeOutCommand
    if(cmd_s == "timeout") {
        Command* cmd = new TimeoutCommand(cmd_line_cpy.c_str(), args_built_in);
        return cmd;
    }

    string tmp(cmd_line);
    int count_1 = count(tmp.begin(), tmp.end(), '>');
    string red_append = ">>";
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    int pos = 0;
    int count_2 = 0;
    while ((pos = tmp.find(red_append, pos )) != std::string::npos) {
        ++ count_2;
        pos += red_append.length();
    }
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    if(count_1 == 1 && count_2 == 2) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        size_t pos_begin = args_built_in[0].find(">>");
        size_t pos_end = args_built_in[num_args1-1].find(">>");
        if(pos_begin != 0 && (pos_end != args_built_in[num_args1-1].length()-2)) {
            Command* cmd = new RedirectionCommand(cmd_line_cpy.c_str());
            return cmd;
        }
    }

    if(count_1 == 0 && count_2 == 1) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        size_t pos_begin = args_built_in[0].find(">");
        size_t pos_end = args_built_in[num_args1-1].find(">");
        if(pos_begin != 0 && (pos_end != args_built_in[num_args1-1].length()-1)) {
            Command* cmd = new RedirectionCommand(cmd_line_cpy.c_str());
            return cmd;
        }
    }

    int count_3 = count(tmp.begin(), tmp.end(), '|');
    if(count_3 == 1) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        size_t pos_begin = args_built_in[0].find("|");
        size_t pos_end = args_built_in[num_args1-1].find("|");
        if(pos_begin != 0 && (pos_end != args_built_in[num_args1-1].length()-1)) {
            Command* cmd = new PipeCommand(cmd_line_cpy.c_str());
            return cmd;
        }
    }

    if(cmd_s == "chprompt"||cmd_s == "chprompt&" ) {
        Command* cmd = new chprompt(cmd_line, args_built_in, num_args1);
        return cmd;
    }



    if(cmd_s == "showpid"||cmd_s == "showpid&") {
        Command* cmd = new ShowPidCommand(cmd_line);
        return cmd;
    }

    if(cmd_s == "pwd"||cmd_s == "pwd&") {
        Command* cmd = new GetCurrDirCommand(cmd_line);
        return cmd;
    }

    if(cmd_s == "cd"||cmd_s == "cd&") {
        Command* cmd = new ChangeDirCommand(cmd_line, args_built_in, num_args1);
        return cmd;
    }

    if(cmd_s == "jobs"||cmd_s == "jobs&") {
        Command* cmd = new JobsCommand(cmd_line);
        return cmd;
    }

    if(cmd_s == "kill"||cmd_s == "kill&") {
        Command *cmd = new KillCommand(cmd_line, args_built_in);
        return cmd;
    }

    if(cmd_s == "fg"||cmd_s == "fg&") {
        Command *cmd = new ForegroundCommand(cmd_line, args_built_in);
        return cmd;
    }

    if(cmd_s == "bg"||cmd_s == "bg&") {
        Command *cmd = new BackgroundCommand(cmd_line, args_built_in);
        return cmd;
    }

    if(cmd_s == "quit"||cmd_s == "quit&") {
        Command *cmd = new QuitCommand(cmd_line, args_built_in);
        return cmd;
    }

    if(cmd_s == "head"||cmd_s == "head&") {
        Command *cmd = new HeadCommand(cmd_line_cpy.c_str());
        return cmd;
    }

    Command *cmd = new ExternalCommand(cmd_line_cpy.c_str());
    return cmd;

    return nullptr;
}

void SmallShell::executeCommand(const char *cmd_line) {
    Command* cmd = CreateCommand(cmd_line);
    cmd->execute();
    delete cmd;
    return;
}

chprompt::chprompt(const char* cmd_line, vector<string>& args, int num_args): BuiltInCommand(cmd_line) {
    if(num_args == 1)
        prompt = "smash> ";
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    if(num_args > 1) {
        prompt = args[1];
        prompt += "> ";
    }
}

void chprompt::execute() {
    SmallShell::prompt = prompt;
}


ShowPidCommand::ShowPidCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void ShowPidCommand::execute() {
	auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    std::cout << "smash pid is " << SmallShell::smash_pid << std::endl;
}

GetCurrDirCommand::GetCurrDirCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void GetCurrDirCommand::execute() {
    char* cur_dir = get_current_dir_name();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    if(!cur_dir) {
        perror("smash error: gcd failed");
        return;
    }
    std::cout << cur_dir << std::endl;
    free(cur_dir);
}

ChangeDirCommand::ChangeDirCommand(const char *cmd_line, vector<string> &args, int num_args) : BuiltInCommand(cmd_line), args(args), num_args(num_args) {}

void ChangeDirCommand::execute() {
    if(num_args > 2 || num_args == 1) {
	auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        std::cout << "smash error: cd: too many arguments" << std::endl;
        return;
    }
    if((SmallShell::prev_dir.length() == 0) && (args[1] == "-")) {
	auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        std::cout << "smash error: cd: OLDPWD not set" << std::endl;
        return;
    }
    char* tmp = get_current_dir_name();
    if(!tmp) {
        perror("smash error: get_current_dir_name failed");
        return;
    }
    if(args[1] == "-") {
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        args[1] = SmallShell::prev_dir;
    }
    const char* path = args[1].c_str();
    int res = chdir(path);
    if(res < 0) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        perror("smash error: chdir failed");
        return;
    }
    SmallShell::prev_dir = tmp;
    free(tmp);
}

JobsCommand::JobsCommand(const char *cmd_line) : BuiltInCommand(cmd_line) {}

void JobsCommand::execute() {
    SmallShell::jobs_list.printJobsList();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
}

KillCommand::KillCommand(const char *cmd_line, vector<string>& args) : BuiltInCommand(cmd_line), args(args) {}

void KillCommand::execute() {
    if(args.size() != 3) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        std::cout << "smash error: kill: invalid arguments" << std::endl;
        return;
    }

    for (int i = 0; i < args[1].size(); i++) {
        if(!(i==0) && !isdigit(args[1][i])) {
            std::cout << "smash error: kill: invalid arguments" << std::endl;
            return;
        }
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    }


    if(args[1][0] != '-') {
        std::cout << "smash error: kill: invalid arguments" << std::endl;
        return;
    }

    for (int i = 0; i < args[2].size(); i++) {
        if ((!isdigit(args[2][i]) && (i != 0)) || ((i == 0) && ((args[2][0] == '-') && (args[2].size() == 1)))) {
            std::cout << "smash error: kill: invalid arguments" << std::endl;
            return;
        }
    }
    string sub_str1 = args[1].substr(1);
    int sig_num1 = stoi(sub_str1);
    int job_id1 =  stoi(args[2]);

    JobsList::JobEntry* job = SmallShell::jobs_list.getJobById(job_id1);
    if(job == NULL) {
        std::cout << "smash error: kill: job-id " << job_id1 << " does not exist" << std::endl;
        return;
    }

    if(killpg(job->job_PID, sig_num1) < 0) {
        perror("smash error: killpg failed");
        return;
    }
    double x=0;
	x++;
	x++;

    std::cout << "signal number " << sig_num1 << " was sent to pid " << job->job_PID << std::endl;

    if(sig_num1 == SIGCONT) {
        job->state = BACKGROUND;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    }
}

ForegroundCommand::ForegroundCommand(const char *cmd_line, vector<string> &args) : BuiltInCommand(cmd_line), args(args) {}

void ForegroundCommand::execute() {
    if(args.size() > 2) {
        std::cout << "smash error: fg: invalid arguments" << std::endl;
        return;
    }
    string cmd_line;
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    pid_t pid;
    int job_id;
    JOB_STATE state;
    JobsList::JobEntry* job;
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    vector<JobsList::JobEntry>::iterator ptr;
    if(args.size()==2) {
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        for (int i = 0; i < args[1].size(); i++) {
            if ((!isdigit(args[1][i]) && (i != 0)) || ((i == 0) && ((args[1][0] == '-') && (args[1].size() == 1)))) {
                std::cout << "smash error: fg: invalid arguments" << std::endl;
                return;
            }
        }
        job_id = stoi(args[1]);
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        job = SmallShell::jobs_list.getJobById(job_id);
        if(job == NULL) {
            std::cout << "smash error: fg: job-id " << job_id << " does not exist" << std::endl;
            return;
        }
        ptr = SmallShell::jobs_list.getItrByJobId(job_id);
        cmd_line = job->cmd_line;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        pid = job->job_PID;
        state = job->state;
    }
    if(args.size() == 1) {
        if(SmallShell::jobs_list.jobs_list.size()  == 0) {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            std::cout << "smash error: fg: jobs list is empty" << std::endl;
            return;
        }
    }
    if(args.size() == 1) {
        ptr = SmallShell::jobs_list.jobs_list.end();
        ptr--;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        cmd_line = ptr->cmd_line;
        pid = ptr->job_PID;
        state = ptr->state;
    }

    std::cout << cmd_line << " : " << pid << std::endl;
    if(state == STOPPED) {
        if(killpg(pid, SIGCONT) < 0) {
            perror("smash error: killpg failed");
            return;
        }
    }
    SmallShell::smash_fg_son = pid;
    SmallShell::cmd_line_signal = cmd_line;
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    int status = 0;
    pid_t temp_pid = waitpid(pid, &status, WUNTRACED);
    if(temp_pid == -1) {
        perror("smash error: waitpid failed");
        return;
    }
    if(WIFSTOPPED(status) || WIFSIGNALED(status))
        return;
    if(temp_pid == pid) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        SmallShell::jobs_list.jobs_list.erase(ptr);
        SmallShell::smash_fg_son = 0;
        SmallShell::cmd_line_signal = "";
        for (vector<TimeOutNode>::iterator itr = SmallShell::time_out_vec.begin(); itr < SmallShell::time_out_vec.end() && (SmallShell::time_out_vec.size() != 0); itr++) {
            if( itr->pid == pid)
                SmallShell::time_out_vec.erase(itr);
        }
    }

}

BackgroundCommand::BackgroundCommand(const char *cmd_line, vector<string> &args) : BuiltInCommand(cmd_line)
        , args(args) {}

void BackgroundCommand::execute() {
    if(args.size() > 2) {
        std::cout << "smash error: bg: invalid arguments" << std::endl;
        return;
    }
    string cmd_line;
    pid_t pid;
    int job_id;
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2(); 
    double x=0;
    x++;
    x++;
    if(x<0) return;
    JOB_STATE state;
    JobsList::JobEntry* job;
    if(args.size()==2) {
        for (int i = 0; i < args[1].size(); i++) {
            if ((!isdigit(args[1][i]) && (i != 0)) || ((i == 0) && ((args[1][0] == '-') && (args[1].size() == 1)))) {
                std::cout << "smash error: bg: invalid arguments" << std::endl;
                return;
            }
        }
        job_id = stoi(args[1]);
        job = SmallShell::jobs_list.getJobById(job_id);
        if(job == NULL) {
            std::cout << "smash error: bg: job-id " << job_id << " does not exist" << std::endl;
            return;
        }
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        cmd_line = job->cmd_line;
        pid = job->job_PID;
        state = job->state;

        if(state == BACKGROUND) {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            std::cout << "smash error: bg: job-id " << job_id << " is already running in the background" << std::endl;
            return;
        }
    }

    if(args.size() == 1) {
        job = SmallShell::jobs_list.getLastStoppedJob(&job_id);
        if(job == NULL) {
            std::cout << "smash error: bg: there is no stopped jobs to resume" << std::endl;
            return;
        }
        cmd_line = job->cmd_line;
        pid = job->job_PID;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    }

    std::cout << cmd_line << " : " << pid << std::endl;
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    if(killpg(pid, SIGCONT) < 0) {
        perror("smash error: killpg failed");
        return;
    }
    job->state = BACKGROUND;
}

QuitCommand::QuitCommand(const char* cmd_line, vector<string> args) : BuiltInCommand(cmd_line),
                                                                      args(args) {}

void QuitCommand::execute() {
	double z=0;
	z++;
	z++;
    if((args.size() >= 2) && args[1] == "kill") {
        std::cout << "smash: sending SIGKILL signal to " << SmallShell::jobs_list.jobs_list.size() << " jobs:" << std::endl;
        for (vector<JobsList::JobEntry>::iterator ptr = SmallShell::jobs_list.jobs_list.begin(); ptr < SmallShell::jobs_list.jobs_list.end(); ++ptr) {
            std::cout << ptr->job_PID << ": " << ptr->cmd_line << std::endl;
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        }
    }
    SmallShell::jobs_list.killAllJobs();
  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    SmallShell::exit_flag_quit = true;
}

ExternalCommand::ExternalCommand(const char *cmd_line) : Command(cmd_line) {}

void ExternalCommand::execute() {
    bool bg = _isBackgroundComamnd(get_cmd_line().c_str());
    string str_tmp(get_cmd_line());
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    _removeBackgroundSign((char*)str_tmp.c_str());

    char** argv = (char**) malloc(4 * sizeof(char*));
    argv[0] = (char*)malloc(10 * sizeof(char));
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    argv[1] = (char*)malloc(3 * sizeof(char));
    argv[2] = (char*)malloc((strlen(str_tmp.c_str()) + 1) * sizeof(char));

    argv[0] = strcpy(argv[0], "/bin/bash");
    argv[1] = strcpy(argv[1], "-c");
      auxi2();
    auxi2();
    double x=0;
	x++;
	x++;
    auxi2();
    auxi2();
    auxi2();
    argv[2] = strcpy(argv[2], str_tmp.c_str());
    argv[3] = NULL;

    pid_t pid = fork();
    if(pid == -1) {
        perror("smash error: fork failed");
        return;
    }
    if(pid == 0) {
        if(SmallShell::set_grp) {
            if (setpgrp() == -1) {
				  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                perror("smash error: setpgrp failed");
                return;
            }
        }

        if(execv(argv[0], argv) == -1) {
            for(int i=0; i<3; i++)
                free(argv[i]);
            free(argv);
        }
    }
    if(pid > 0) {
        if(bg) {
            SmallShell::jobs_list.addJob(this, pid);
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        }

        else {
            SmallShell::smash_fg_son = pid;
            SmallShell::cmd_line_signal = this->get_cmd_line();
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            if(SmallShell::set_grp) {
                if (waitpid(pid, NULL, WUNTRACED) == -1) {
                    perror("smash error: waitpid failed");
                    return;
                }
            }
            else if(!SmallShell::set_grp) {
				  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                    wait(NULL);
            }
            SmallShell::smash_fg_son = 0;
            SmallShell::cmd_line_signal = "";
        }
        for(int i=0; i<3; i++)
            free(argv[i]);
        free(argv);
    }
    return;
}

RedirectionCommand::RedirectionCommand(const char *cmd_line) : Command(cmd_line) {
    string str(cmd_line);
    string file_tmp;
    string cmd_tmp;
    double x=0;
	x++;
	x++;
    size_t pos = str.find(">>");
    bool append_tmp = true;
    if(pos == string::npos) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        append_tmp = false;
        pos = str.find(">");
        cmd_tmp = str.substr(0, pos);
        file_tmp = str.substr(pos + 1);
    }
    else {
        cmd_tmp = str.substr(0, pos);
        file_tmp = str.substr(pos + 2);
    }
    background = _isBackgroundComamnd(cmd_line);
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    _removeBackgroundSign((char*)file_tmp.c_str());
    split(cmd_tmp.c_str(), inner_cmd_args);
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    split(file_tmp.c_str(), inner_file_args);
    file = inner_file_args[0];
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    _removeBackgroundSign((char*)file.c_str());
    inner_cmd_line = cmd_tmp;
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    append = append_tmp;
}

void RedirectionCommand::execute() {
    int fd_tmp = open(file.c_str(), O_APPEND | O_CREAT | O_RDWR, 0666);
 
    if(close(fd_tmp) == -1) {
        perror("smash error: close failed");
        return;
    }

    string builtin_chk = inner_cmd_args[0];
    if(builtin_chk == "chprompt" || builtin_chk == "showpid" || builtin_chk == "pwd"
       || builtin_chk == "head" || builtin_chk == "jobs" || builtin_chk == "quit" || builtin_chk == "kill"
       || builtin_chk == "fg" || builtin_chk == "bg") {
		     auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    double z=0;
	z++;
	z++;
        int screen_fd;
        screen_fd = dup(1);
        if(screen_fd == -1) {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            perror("smash error: dup failed");
            return;
        }
        if(close(1) == -1) {
            perror("smash error: close failed");
            return;
        }
        if(append) {
            if (open(file.c_str(), O_APPEND | O_CREAT | O_RDWR, 0666) == -1) {
                perror("smash error: open failed");
                  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                return;
            }
        }
        else {
            if (open(file.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666) == -1) {
                perror("smash error: open failed");
                  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                return;
            }
        }
        SmallShell& tmp = SmallShell::getInstance();
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        tmp.executeCommand(inner_cmd_line.c_str());
        if(close(1) == -1) {
            perror("smash error: close failed");
            return;
        }
        if(dup2(screen_fd, 1) == -1) {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            perror("smash error: dup2 failed");
            return;
        }
        if(close(screen_fd) == -1) {
			
            perror("smash error: close failed");
            return;
        }
        return;
    }

    pid_t pid = fork();
    if(pid == -1) {
        perror("smash error: pid failed");
        return;
    }
    if (pid == 0) {
        SmallShell::set_grp = false;
        SmallShell::exit_flag_redirect = true;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        if(setpgrp() == -1) {
            perror("smash error: setpgrp failed");
            return;
        }
        if(close(1) == -1) {
            perror("smash error: close failed");
            return;
        }
        if(append) {
            if (open(file.c_str(), O_APPEND | O_CREAT | O_RDWR, 0666) == -1) {
                perror("smash error: open failed");
                return;
            }
        }
        else {
            if (open(file.c_str(), O_CREAT | O_RDWR | O_TRUNC, 0666) == -1) {
                perror("smash error: open failed");
                return;
            }
        }
        SmallShell& tmp = SmallShell::getInstance();
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        tmp.executeCommand(inner_cmd_line.c_str());
        if(close(1) == -1) {
            perror("smash error: close failed");
            return;
        }
        return;
    }

    if(pid > 0) {
        if(background) {
            SmallShell::jobs_list.addJob(this, pid);
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        }
        else {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            SmallShell::smash_fg_son = pid;
            SmallShell::cmd_line_signal = this->get_cmd_line();
            if(SmallShell::set_grp) {
                if (waitpid(pid, NULL, WUNTRACED) == -1) {
                    perror("smash error: waitpid failed");
                    return;
                }
            }
           else if(!SmallShell::set_grp) {
                wait(NULL);
                  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            }
            SmallShell::smash_fg_son = 0;
            SmallShell::cmd_line_signal = "";
        }
        return;
    }
}


TimeOutNode::TimeOutNode(time_t start_time, time_t duration, string cmd_line, pid_t pid, bool killed) : 
start_time(start_time), duration(duration), cmd_line(cmd_line), pid(pid), killed(killed){}

TimeoutCommand::TimeoutCommand(const char *cmd_line, vector<string>& args) : Command(cmd_line), args(args) {}

class Comp {
	bool pos=0;
	bool firs=0;
public:
    bool operator()(time_t dur, TimeOutNode node) {
        time_t time_now = time(NULL);
        if (time_now == -1) {
            perror("smash error: time failed");
            return false;
        }
        return dur < (node.duration - difftime(time_now, node.start_time));
    }
};

void TimeoutCommand::execute() {
    if(args.size() < 3) {
        std::cout << "smash error: timeout: invalid arguments" << std::endl;
        return;
    }
    duration = stod(args[1]);
    inner_cmd = this->get_cmd_line();
    double z=0;
	z++;
	z++;
    inner_cmd = _ltrim(inner_cmd);
    inner_cmd.replace(0, args[0].size(), "");
    double x=0;
	x++;
	x++;
    inner_cmd = _ltrim(inner_cmd);
    inner_cmd.replace(0, args[1].size(), "");
    inner_cmd = _ltrim(inner_cmd);
    for (int i = 0; i < args[1].size(); i++) {
        if (!isdigit(args[1][i])) {
            std::cout << "smash error: timeout: invalid arguments" << std::endl;
            return;
        }
    }

    time_t time_now = time(NULL);
    if(time_now == -1) {
        perror("smash error: time failed");
        return;
    }
    if((SmallShell::time_out_vec.size() == 0) || duration < (SmallShell::time_out_vec[0].duration - difftime(time_now, SmallShell::time_out_vec[0].start_time))) {
        alarm(duration);
    }

    vector<string> tmp;
    split(inner_cmd.c_str(), tmp);
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    string builtin_chk = tmp[0];
    SmallShell& smash = SmallShell::getInstance();
    Comp comp;
    if(builtin_chk == "chprompt" || builtin_chk == "showpid" || builtin_chk == "pwd"
       || builtin_chk == "head" || builtin_chk == "jobs" || builtin_chk == "quit" || builtin_chk == "kill"
       || builtin_chk == "fg" || builtin_chk == "bg") {
        TimeOutNode builtin_node(time_now, duration, "");
        vector<TimeOutNode>::iterator pos = upper_bound(SmallShell::time_out_vec.begin(), SmallShell::time_out_vec.end(),
                                                        duration,  comp);
        SmallShell::time_out_vec.insert(pos, builtin_node);
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        smash.executeCommand(inner_cmd.c_str());
        return;
    }

    pid_t timeout_pid = fork();
    if(timeout_pid == -1) {
        perror("smash error: fork failed");
        return;
    }
    if (timeout_pid == 0) {
        SmallShell::set_grp = false;
        SmallShell::exit_flag_time_out = true;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        if(setpgrp() == -1) {
            perror("smash error: setpgrp failed");
            return;
        }
        _removeBackgroundSign((char*)inner_cmd.c_str());
          auxi2();
    auxi2();
    auxi2();
    double z=0;
	z++;
	z++;
    auxi2();
    auxi2();
        smash.executeCommand(inner_cmd.c_str());

        return;
    }


    if(timeout_pid > 0) {
        TimeOutNode builtin_node(time_now, duration, get_cmd_line(), timeout_pid);
        vector<TimeOutNode>::iterator pos = upper_bound(SmallShell::time_out_vec.begin(), SmallShell::time_out_vec.end(),
                                                        duration, comp);
        SmallShell::time_out_vec.insert(pos, builtin_node);
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        bool background = _isBackgroundComamnd(get_cmd_line().c_str());
        if(background) {
            SmallShell::jobs_list.addJob(this, timeout_pid);
        }
        else {
            SmallShell::smash_fg_son = timeout_pid;
            SmallShell::cmd_line_signal = this->get_cmd_line();
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            int status = 0;
            status+=0;
            if(SmallShell::set_grp) {
                if (waitpid(timeout_pid, &status, WUNTRACED) == -1) {
                    perror("smash error: waitpid failed");
                    return;
                }
            }
            else if(!SmallShell::set_grp)
                wait(NULL);
                  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            SmallShell::smash_fg_son = 0;
            SmallShell::cmd_line_signal = "";

            if(WIFSTOPPED(status) || WIFSIGNALED(status)) {
				  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                return;
            }
            for (vector<TimeOutNode>::iterator itr = SmallShell::time_out_vec.begin();
                 itr < SmallShell::time_out_vec.end() && (SmallShell::time_out_vec.size() != 0); itr++) {
                if(itr->pid == timeout_pid)
                    itr->killed = true;
            }
        }
        return;
    }
}


bool isItNumber (char* str){
    if (strcmp("",str) == 0) {
        return false;
    }
    int i;
    if(str[0] == '-'){
        i=1;
    }
    else {
        i = 0;
    }
    while(str[i] != '\0'){
        if((str[i] < '0' || str[i] >'9') ){
            return  false;
        }
        i++;
    }
    return  true;
}



PipeCommand::PipeCommand(const char *cmd_line) : Command(cmd_line) {
    string str(cmd_line);
    string cmd_line_1_tmp;
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    string cmd_line_2_tmp;
    size_t pos = str.find("|&");
    stderr_flag = true;
    if(pos == string::npos) {
		  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        stderr_flag = false;
        pos = str.find("|");
        double x=0;
	x++;
	x++;
        cmd_line_1_tmp = str.substr(0, pos);
        cmd_line_2_tmp = str.substr(pos + 1);
    }
    else {
        cmd_line_1_tmp = str.substr(0, pos);
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        cmd_line_2_tmp = str.substr(pos + 2);
    }
    background = _isBackgroundComamnd(cmd_line);
    _removeBackgroundSign((char*)cmd_line_2_tmp.c_str());
    _removeBackgroundSign((char*)cmd_line_1_tmp.c_str());
      auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
    cmd_line_1 = cmd_line_1_tmp;
    double z=0;
	z++;
	z++;
    cmd_line_2 = cmd_line_2_tmp;
}

void PipeCommand::execute() {
    pid_t pipe_father = fork();
    if(pipe_father == -1) {
        perror("smash error: fork failed");
        return;
    }
    if(pipe_father == 0) {
        SmallShell::exit_flag_pipe = true;
        SmallShell::set_grp = false;
        if (setpgrp() == -1) {
            perror("smash error: setpgrp failed");
            return;
        }
        int my_pipe[2];
        pipe(my_pipe);
        pid_t pid_1 = fork();
        pid_t pid_2;
          auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        if (pid_1 == -1) {
            perror("smash error: fork failed");
            return;
        }
        if (pid_1 > 0) {
            pid_2 = fork();
            if (pid_2 == -1) {
				  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                perror("smash error: fork failed");
                return;
            }
        }

        if (pid_1 > 0 && pid_2 > 0) {
            if (close(my_pipe[0]) == -1) {
                perror("smash error: close failed");
                return;
            }
            if (close(my_pipe[1]) == -1) {
                perror("smash error: close failed");
                return;
            }
        }

        if (pid_1 == 0) {
            SmallShell::exit_flag_pipe = true;
            if (!stderr_flag) {
				  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
                if (close(1) == -1) {
                    perror("smash error: close failed");
                    return;
                }
            } else {
                if (close(2) == -1) {
                    perror("smash error: close failed");
                    return;
                }
            }

            if (close(my_pipe[0]) == -1) {
                perror("smash error: close failed");
                return;
            }

            if (dup(my_pipe[1]) == -1) {
                perror("smash error: dup failed");
                return;
            }
            if (close(my_pipe[1]) == -1) {
                perror("smash error: close failed");
                return;
            }
            SmallShell &tmp = SmallShell::getInstance();
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            tmp.executeCommand(cmd_line_1.c_str());

            return;
        }

        if (pid_2 == 0) {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            SmallShell::exit_flag_pipe = true;
            if (close(0) == -1) {
                perror("smash error: close failed");
                return;
            }

            if (close(my_pipe[1]) == -1) {
                perror("smash error: close failed");
                return;
            }

            if (dup(my_pipe[0]) == -1) {
                perror("smash error: dup failed");
                return;
            }
            if (close(my_pipe[0]) == -1) {
                perror("smash error: close failed");
                return;
            }

            SmallShell &tmp = SmallShell::getInstance();
            double z=0;
	z++;
	z++;
            tmp.executeCommand(cmd_line_2.c_str());

            return;
        }

        //pipe_father
        if(waitpid(pid_1, NULL, 0) == -1) {
            perror("smash error: waitpid failed");
            return;
        }
        if(waitpid(pid_2, NULL, 0) == -1) {
            perror("smash error: waitpid failed");
            return;
        }

        return;
    }

    //smash
    if(pipe_father > 0) {
        if(background) {
            SmallShell::jobs_list.addJob(this, pipe_father);
        }
        else {
			  auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            SmallShell::smash_fg_son = pipe_father;
            SmallShell::cmd_line_signal = this->get_cmd_line();
            if(SmallShell::set_grp) {
                if (waitpid(pipe_father, NULL, WUNTRACED) == -1) {
                    perror("smash error: waitpid failed");
                    return;
                }
            }
            else if(!SmallShell::set_grp) {
                wait(NULL);
            }
              auxi2();
    auxi2();
    auxi2();
    auxi2();
    auxi2();
            SmallShell::smash_fg_son = 0;
            SmallShell::cmd_line_signal = "";
        }
        return;
    }
}


int _parseCommandLine(const char* cmd_line, char** args) {
  FUNC_ENTRY()
  int i = 0;
  std::istringstream iss(_ltrim(string(cmd_line)).c_str());
  for(std::string s; iss >> s; ) {
    args[i] = (char*)malloc(s.length()+1);
    memset(args[i], 0, s.length()+1);
    strcpy(args[i], s.c_str());
    args[++i] = NULL;
  }
  return i;

  FUNC_EXIT()
}

HeadCommand::HeadCommand(const char* cmd_line) :BuiltInCommand(cmd_line),cmd(cmd_line){}


 void HeadCommand::execute(){




	char* args[20];
    int len_of_cmd = _parseCommandLine(cmd, args);

    if (len_of_cmd != 3 && len_of_cmd != 2) {
        cerr<< "smash error: head: not enough arguments"<<endl;
        return;
    }
    int i=1;
    int lines=10;
    if (len_of_cmd == 3 ) {
        lines=atoi(args[1]+1);
        i=2;
    }

        if (_isBackgroundComamnd(cmd) == 1) {
            _removeBackgroundSign(args[2]);
        }


	//  int signal_num = atoi((args[1]) + 1);
     //   int jobId = atoi(args[2]);

		 char one_char[1];

			 int res;
                    int opened_file= open(args[i],O_RDONLY,0666);
					if(opened_file==-1){
						perror("smash error: open failed");
						return;
						}

						ssize_t num_of_bytes=read(opened_file,one_char,1);
						while(num_of_bytes>0 && lines){
							if(num_of_bytes==-1){
								perror("smash error: read failed");
								return;
							}
							res=write(1,one_char,num_of_bytes);
							if(res==-1){
								perror("smash error: write failed");
								return;
								}
                            if (*one_char == '\n') { lines--;}
							num_of_bytes=read(opened_file,one_char,1);
						}

					res= close(opened_file);
					if(res==-1){
								perror("smash error: close failed");
								return;
								}






			return;





			}


JobsList::JobEntry::JobEntry(int job_id, pid_t job_PID, JOB_STATE state, time_t time, string cmd_line
,bool isStopped,bool isForked) :
        job_id(job_id), job_PID(job_PID), state(state), time(time), cmd_line(cmd_line) {}

JobsList::JobsList() : max_job_id(0) {}

JobsList::JobEntry *JobsList::getLastStoppedJob(int *jobId) {
    if(jobs_list.empty())
        return NULL;
    vector<JobEntry>::iterator ptr1 = jobs_list.end();
    ptr1--;
    ptr1++;
    ptr1--;
     auxi2();
    auxi2();
    auxi2();
    auxi2();
    for (; ptr1 >= jobs_list.begin(); ptr1--) {
        if(ptr1->state == STOPPED)
            return &*ptr1;
    }
    return NULL;
}
void JobsList::addJob(Command* cmd, pid_t pid, 
bool isStopped,string cmd_line) {
	double x=0;
	x++;
	x++;
    JOB_STATE state;
     auxi2();
    auxi2();
    auxi2();
    auxi2();
    if (isStopped) {
        state = STOPPED;
    }
    else {
        state = BACKGROUND;
         auxi2();
    auxi2();
    auxi2();
    auxi2();
    }
    time_t t = time(NULL);
    if (t == (time_t)-1) {
        perror("smash error: time failed");
        return;
    }
    JobsList::JobEntry new_job(max_job_id + 1, pid, state, t, cmd_line);
    if(cmd_line == "") {
        JobsList::JobEntry new_job_tmp(max_job_id+1, pid, state, t, cmd->get_cmd_line());
        new_job = new_job_tmp;
    }
    max_job_id++;
    jobs_list.push_back(new_job);
}

void JobsList::printJobsList() {
	 auxi2();
    auxi2();
    auxi2();
    auxi2();
    removeFinishedJobs();
    for (vector<JobEntry>::iterator ptr = jobs_list.begin(); ptr < jobs_list.end(); ptr++) {
        std::cout << "[" << ptr->job_id << "]" << " " << ptr->cmd_line << " : " << ptr->job_PID << " ";
        time_t t_now = time(NULL);
         auxi2();
    auxi2();
    auxi2();
    auxi2();
        if (t_now == (time_t) -1) {
            perror("smash error: time failed");
            return;
        }
        std::cout << difftime(t_now, ptr->time) << " secs";
         auxi2();
    auxi2();
    auxi2();
    auxi2();
        if (ptr->state == STOPPED)
            std::cout << " (stopped)" << std::endl;
        if (ptr->state != STOPPED)
            std::cout << std::endl;
    }
}

void JobsList::killAllJobs() {
    for (vector<JobEntry>::iterator ptr = jobs_list.begin(); ptr < jobs_list.end(); ptr++) {
        if (killpg(ptr->job_PID, SIGKILL) == -1) {
            perror("smash error: killpg failed");
            return;
        }
    }
     auxi2();
    auxi2();
    auxi2();
    auxi2();
    jobs_list.clear();
    max_job_id = 0;
}

void JobsList::removeFinishedJobs() {
    vector<pid_t> tmp;
    for (vector<JobEntry>::iterator ptr1 = jobs_list.begin(); ptr1 < jobs_list.end() && (jobs_list.size() != 0); ptr1++) {
        int status = 0;
        pid_t pid = waitpid(ptr1->job_PID, &status, WNOHANG | WUNTRACED | WCONTINUED );
    auxi2();
    auxi2();
    auxi2();
    auxi2();
        if(pid == -1) {
            perror("smash error: waitpid failed");
            return;
        }


        if(pid == ptr1->job_PID && !WIFCONTINUED(status) && !WIFSTOPPED(status)) {
            tmp.push_back(pid);
            jobs_list.erase(ptr1);
        }
    }

    for (vector<TimeOutNode>::iterator itr = SmallShell::time_out_vec.begin(); itr < SmallShell::time_out_vec.end() && (SmallShell::time_out_vec.size() != 0); itr++) {
        if (find(tmp.begin(), tmp.end(), itr->pid) != tmp.end()) {
            SmallShell::time_out_vec.erase(itr);
        }
    }


    for (vector<TimeOutNode>::iterator itr1 = SmallShell::time_out_vec.begin(); itr1 < SmallShell::time_out_vec.end() 
    && (SmallShell::time_out_vec.size() != 0); itr1++) {
        int status1 = 0;
        pid_t pid1;
         auxi2();
    auxi2();
    auxi2();
    auxi2();
        if(!itr1->killed) {
            pid1 = waitpid(itr1->pid, &status1, WNOHANG);
            if (pid1 == -1) {
                perror("smash error: waitpid failed");
                return;
            }
            if(pid1 == itr1->pid)
                itr1->killed = true;
     
        }
        if(itr1->killed) {
            SmallShell::time_out_vec.erase(itr1);
             auxi2();
    auxi2();
    auxi2();
    auxi2();
        }
    }

    if(jobs_list.size() != 0) {
        vector<JobEntry>::iterator ptr1 = jobs_list.end();
        ptr1--;
         auxi2();
    auxi2();
    auxi2();
    auxi2();
        max_job_id = ptr1->job_id;
    }
    else
        max_job_id = 0;
}

JobsList::JobEntry* JobsList::getJobById(int jobId){
	 auxi2();
    auxi2();
    auxi2();
    auxi2();
    for (vector<JobEntry>::iterator ptr = jobs_list.begin(); ptr < jobs_list.end(); ptr++){
        if(ptr->job_id == jobId)
            return &*ptr;
    }
    return NULL;
}

vector<JobsList::JobEntry>::iterator JobsList::getItrByJobId(int jobId) {
    for (vector<JobEntry>::iterator ptr = jobs_list.begin(); ptr < jobs_list.end(); ptr++) {
        if(ptr->job_id == jobId)
            return ptr;
    }
     auxi2();
    auxi2();
    auxi2();
    auxi2();
}

