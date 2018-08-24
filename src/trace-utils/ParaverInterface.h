/*
 * ParaverInterface.h
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef PARAVERINTERFACE_H
#define PARAVERINTERFACE_H

#include <string>

class ParaverInterface
{
    public:
        ParaverInterface(std::string tracepath, long long duration);
        void OpenTrace(std::string cfg="mpi/views/MPI_call.cfg");
        void Zoom(unsigned int from, unsigned int to, bool new_tl);
    private:
        void runParaver();
        void knockParaver(unsigned int signal);

        std::string tracepath;
        long long traceduration;
        std::string paraload_file_name="/home/jmartinez/paraload.sig";
        std::string paraver_conf_files="/home/jmartinez/BSC/software/paraver/cfgs/";
        std::string paraver_configuration="mpi/views/MPI_call.cfg";
        std::string paraver_bin_cmd="wxparaver";
        std::string paraver_bin_path="/home/jmartinez/Programas/paraver-gui/bin/wxparaver";
        pid_t paraverpid;

};

#endif /* !PARAVERINTERFACE_H */
