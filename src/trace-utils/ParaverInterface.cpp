/*
 * ParaverInterface.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "ParaverInterface.h"
#include <sys/types.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

ParaverInterface::ParaverInterface(std::string tracepath, long long duration)
    : tracepath(tracepath)
    , traceduration(duration)
    , paraverpid(0)
{
    char * real_path = realpath(tracepath.c_str(), NULL);
    this->tracepath = std::string(real_path);
}

void ParaverInterface::OpenTrace(std::string cfg)
{
    this->paraver_configuration = cfg;
    std::ofstream pf;
    pf.open(this->paraload_file_name);
    pf  << this->paraver_conf_files << cfg << "\n"
        << "0:" << this->traceduration << "\n";
    //    << this->tracepath << "\n";
    pf.close();

    this->runParaver();
    this->knockParaver(SIGUSR1);
}

void ParaverInterface::Zoom(unsigned int from, unsigned int to, bool new_tl)
{
    std::ofstream pf;
    pf.open(this->paraload_file_name);
    pf  << this->paraver_conf_files << this->paraver_configuration << "\n"
        << from << ":" << to << "\n";
    //    << this->tracepath << "\n";
    pf.close();

    this->runParaver();
    if (new_tl)
        this->knockParaver(SIGUSR1);
    else
        this->knockParaver(SIGUSR2);
}

void ParaverInterface::runParaver()
{
    if (this->paraverpid != 0)
    {
        if(kill(this->paraverpid, 0) == 0)
            return;
            //if(errno != ESRCH)
            //    return;
    }
    
    pid_t pid = fork();
    if (pid == -1)
        std::cout << "ERROR ON FORK: " << errno << std::endl;
    if (pid == 0)
    {
        std::cout << this->paraver_bin_cmd << " " << this->tracepath << std::endl;
        execl(this->paraver_bin_path.c_str(), this->paraver_bin_cmd.c_str(), 
                this->tracepath.c_str(), (char*)0);
        std::cout << "Error on execl" << std::endl;
        exit(1);
    } 
    this->paraverpid = pid;
    usleep(1000000); // just wait a bit before send any signal
}

void ParaverInterface::knockParaver(unsigned int signal)
{
    kill(this->paraverpid, signal);
}
