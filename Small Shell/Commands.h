
#ifndef COMMANDS_H_
#define COMMANDS_H_

#include <vector>
#include <string.h>
#include <unistd.h>

using std::string;
using std::endl;
using std::vector;

enum JOB_STATE {BACKGROUND, STOPPED,FORKED};
string _ltrim(const std::string& s);

class JobsList;

class TimeOutNode {
public:
    TimeOutNode();
    TimeOutNode(time_t start_time, time_t duration, string cmd_line, pid_t pid = 0, bool killed = false);
    TimeOutNode(bool killed,bool forked);
    time_t start_time;
    time_t duration;
    string cmd_line;
    pid_t pid;
    bool killed;
    bool forked=false;
    bool ended=false;
};

class Command {
    string cmd_line;
public:
    Command(const char* cmd_line);
    virtual ~Command() {}
    virtual void execute() = 0;
    string& get_cmd_line();
    void setline(string str){cmd_line=str;}
};

class BuiltInCommand : public Command {
public:
    BuiltInCommand(const char* cmd_line);
    virtual ~BuiltInCommand() = default;
};

class ExternalCommand : public Command {
public:
    ExternalCommand(const char* cmd_line);
    virtual ~ExternalCommand() = default;
    void execute() override;
};


class RedirectionCommand : public Command {
    string inner_cmd_line;
    string file;
    vector<string> inner_cmd_args;
    vector<string> inner_file_args;
    bool append;
    bool background;
    bool exe_cmd_line=false;
    bool file2=false;
public:
    explicit RedirectionCommand(const char* cmd_line);
    virtual ~RedirectionCommand() {}
    void execute() override;
};

class PipeCommand : public Command {
    string cmd_line_1;
    string cmd_line_2;
    bool stderr_flag;
    bool background;
    bool foreground=false;
public:
    PipeCommand(const char* cmd_line);
    virtual ~PipeCommand() {}
    void execute() override;
};



class chprompt : public BuiltInCommand {
    string prompt;
public:
    chprompt(const char* cmd_line, vector<string>& args, int num_args);
    virtual ~chprompt() = default;
    void execute() override;
};



class ChangeDirCommand : public BuiltInCommand {
    vector<string> args;
    int num_args;
public:
    ChangeDirCommand(const char* cmd_line, vector<string>& args, int num_args);
    virtual ~ChangeDirCommand() = default;
    void execute() override;
};

class GetCurrDirCommand : public BuiltInCommand {
public:
    GetCurrDirCommand(const char* cmd_line);
    virtual ~GetCurrDirCommand() = default;
    void execute() override;
};

class ShowPidCommand : public BuiltInCommand {
public:
    ShowPidCommand(const char* cmd_line);
    virtual ~ShowPidCommand() = default;
    void execute() override;
};

class QuitCommand : public BuiltInCommand {
    vector<string> args;
public:
    QuitCommand(const char* cmd_line, vector<string> args);
    virtual ~QuitCommand() = default;
    void execute() override;
};

class JobsList {

public:
    class JobEntry {
        // TODO: Add your data members
    public:
        int job_id;
        pid_t job_PID;
        JOB_STATE state;
        time_t time;
        string cmd_line;
        bool isStopped;
        bool isForked;
        JobEntry(int job_id, pid_t job_PID, JOB_STATE state, time_t time, string cmd_line
                   ,bool isStopped=false,bool isForked=false);
        
    };
    // TODO: Add your data members
    vector<JobEntry> jobs_list;
    int max_job_id;
public:
    JobsList();
    void addJob(Command* cmd, pid_t pid,bool isStopped = false ,string cmd_line = "" );
    void printJobsList();
    void killAllJobs();
    void removeFinishedJobs();
    JobEntry* getJobById(int jobId);
    vector<JobEntry>::iterator getItrByJobId(int jobId);
    void removeJobById(int jobId){};
    JobEntry *getLastStoppedJob(int *jobId);
};

class JobsCommand : public BuiltInCommand {
public:
    JobsCommand(const char* cmd_line);
    virtual ~JobsCommand() = default;
    void execute() override;
};

class KillCommand : public BuiltInCommand {
    vector<string> args;
public:
    KillCommand(const char* cmd_line, vector<string>& args);
    virtual ~KillCommand() = default;
    void execute() override;
};

class ForegroundCommand : public BuiltInCommand {
    vector<string> args;
    bool isFg=true;
public:
    ForegroundCommand(const char* cmd_line, vector<string>& args);
    virtual ~ForegroundCommand() = default;
    void execute() override;
};

class BackgroundCommand : public BuiltInCommand {
    bool isBg=true;
    vector<string> args;
public:
    BackgroundCommand(const char* cmd_line, vector<string>& args);
    virtual ~BackgroundCommand() = default;
    void execute() override;
};

class TimeoutCommand : public Command {
    time_t duration;
    string inner_cmd;
    vector<string> args;
public:
    TimeoutCommand(const char* cmd_line, vector<string>& args);
    virtual ~TimeoutCommand() = default;
    void execute() override;
};

class HeadCommand : public BuiltInCommand {
    const char* cmd;
 public:
  HeadCommand(const char* cmd_line);
  virtual ~HeadCommand() {}
  void execute() override;
};


class SmallShell {
private:
    // TODO: Add your data members
    string nameOnShell="smash >";
    string oldPwd="";

    
    SmallShell();
public:
    static JobsList jobs_list;
    static vector<TimeOutNode> time_out_vec;
    static string prompt;
    static string prev_dir;
    static bool set_grp;
    static bool exit_flag_quit;
    static bool exit_flag_redirect;
    static bool exit_flag_pipe;
    static bool exit_flag_time_out;
    static pid_t smash_pid;
    static pid_t smash_fg_son;
    static string cmd_line_signal;
    Command *CreateCommand(const char* cmd_line);
    static SmallShell& getInstance() // make SmallShell singleton
    {
        static SmallShell instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }
    ~SmallShell();
    void executeCommand(const char* cmd_line);
};

#endif //COMMAND_H_
