#include <iostream>
#include <signal.h>
#include "signals.h"
#include "Commands.h"

using namespace std;



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


void ctrlZHandler(int sig_num) {
    std::cout << "smash: got ctrl-Z" << std::endl;
    if(SmallShell::smash_fg_son == 0)
        return;
         auxi2();

    if (killpg(SmallShell::smash_fg_son, SIGSTOP) == -1) {
        perror("smash error: killpg failed");
        return;
    }
     auxi2();
    std::cout << "smash: process " << SmallShell::smash_fg_son << " was stopped" << std::endl;
    for (vector<JobsList::JobEntry>::iterator ptr = SmallShell::jobs_list.jobs_list.begin();
         ptr < SmallShell::jobs_list.jobs_list.end() &&
         (SmallShell::jobs_list.jobs_list.size() != 0); ptr++) {
        if(ptr->job_PID == SmallShell::smash_fg_son) {
            ptr->state = STOPPED;
            auxi2();
            ptr->time = time(NULL);
            if (ptr->time == -1) {
                perror("smash error: time failed");
                return;
            }
            auxi2();
            SmallShell::smash_fg_son = 0;
            auxi2();
            SmallShell::cmd_line_signal = "";
            return;
        }
    }
    SmallShell::jobs_list.addJob(NULL, SmallShell::smash_fg_son, 
    true,SmallShell::cmd_line_signal);
    SmallShell::smash_fg_son = 0;
    SmallShell::cmd_line_signal = "";
}

void ctrlCHandler(int sig_num) {
    std::cout << "smash: got ctrl-C" << std::endl;
    if(SmallShell::smash_fg_son == 0)
        return;
    if (killpg(SmallShell::smash_fg_son, SIGKILL) == -1) {
        perror("smash error: killpg failed");
        return;
    }

    std::cout << "smash: process " << SmallShell::smash_fg_son << " was killed" << std::endl;

    for (vector<JobsList::JobEntry>::iterator ptr = SmallShell::jobs_list.jobs_list.begin(); ptr < SmallShell::jobs_list.jobs_list.end() && (SmallShell::jobs_list.jobs_list.size() != 0); ptr++) {
        if(SmallShell::smash_fg_son == ptr->job_PID)
            SmallShell::jobs_list.jobs_list.erase(ptr);
          auxi2();
    }
    for (vector<TimeOutNode>::iterator itr = SmallShell::time_out_vec.begin(); itr < SmallShell::time_out_vec.end() && (SmallShell::time_out_vec.size() != 0); itr++) {
        if (itr->pid == SmallShell::smash_fg_son)
            SmallShell::time_out_vec.erase(itr);
    }

    if(SmallShell::jobs_list.jobs_list.size() != 0) {
        vector<JobsList::JobEntry>::iterator ptr = SmallShell::jobs_list.jobs_list.end();
        ptr--;
        SmallShell::jobs_list.max_job_id = ptr->job_id;
        auxi2();
    }
    else
        SmallShell::jobs_list.max_job_id = 0;

    SmallShell::smash_fg_son = 0;
    SmallShell::cmd_line_signal = "";
    return;
}

void alarmHandler(int sig_num) {
     auxi2();
    SmallShell::jobs_list.removeFinishedJobs();
    std::cout << "smash: got an alarm" << std::endl;
    auxi2();
    time_t time_now = time(NULL);
    if (time_now == -1) {
        perror("smash error: time failed");
        return;
    }
    for (vector<TimeOutNode>::iterator itr = SmallShell::time_out_vec.begin();
         itr < SmallShell::time_out_vec.end() && (SmallShell::time_out_vec.size() != 0); itr++) {
        if (itr->duration - difftime(time_now, itr->start_time) <= 0) {
            if (itr->cmd_line.size() == 0) {
                SmallShell::time_out_vec.erase(itr);
            } else if (itr->pid > 0) {
                if(!itr->killed) {
                    if (killpg(itr->pid, SIGKILL) == -1) {
                        perror("smash error: killpg failed");
                        return;
                    }
                }
                std::cout << "smash: " << itr->cmd_line << " timed out!" << std::endl;
                SmallShell::time_out_vec.erase(itr);
                auxi2();
                for (vector<JobsList::JobEntry>::iterator ptr = SmallShell::jobs_list.jobs_list.begin();
                     ptr < SmallShell::jobs_list.jobs_list.end() &&
                     (SmallShell::jobs_list.jobs_list.size() != 0); ptr++) {
                    if (itr->pid == ptr->job_PID) {
                        SmallShell::jobs_list.jobs_list.erase(ptr);
                    }
                }
            }
        }
    }
    if(SmallShell::time_out_vec.size() != 0)
        alarm(SmallShell::time_out_vec[0].duration - difftime(time_now, SmallShell::time_out_vec[0].start_time));
}

