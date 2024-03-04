#include <iostream>
#include <unistd.h>
#include "Commands.h"
#include "signals.h"
#include <signal.h>



static void auxi1(void)
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

static void HandleHostSignal1(void)
{
    struct sigaction act1;
    auxi1();
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = 0;
    act1.sa_handler = alarmHandler;
    act1.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGALRM, &act1, NULL) < 0) {
        perror ("smash error: sigaction failed");
    }
    struct sigaction act2;
    auxi1();
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = 0;
    act2.sa_handler = ctrlZHandler;
    act2.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGTSTP, &act2, NULL) < 0) {
        perror ("smash error: sigaction failed");
    }

    struct sigaction act3;
    auxi1();
    sigemptyset(&act3.sa_mask);
    act3.sa_flags = 0;
    act3.sa_handler = ctrlCHandler;
    act3.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGINT, &act3, NULL) < 0) {
        perror ("smash error: sigaction failed");
    }
}

int main(int argc, char* argv[]) {

    HandleHostSignal1();
    auxi1();
    SmallShell& smash = SmallShell::getInstance();
    while(true) {
        std::cout << smash.prompt;
        auxi1();
        std::string cmd_line;
        auxi1();
        std::getline(std::cin, cmd_line);
        auxi1();
        string tmp = cmd_line;
        auxi1();
        tmp = _ltrim(tmp);
        if(tmp == "")
            continue;
        smash.jobs_list.removeFinishedJobs();
        auxi1();
        smash.executeCommand(cmd_line.c_str());

        if(SmallShell::exit_flag_quit || SmallShell::exit_flag_redirect || SmallShell::exit_flag_pipe || SmallShell::exit_flag_time_out )
            break;
    }
}


