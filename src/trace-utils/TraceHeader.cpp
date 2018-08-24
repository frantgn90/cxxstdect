/*
 * TraceHeader.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include "TraceHeader.h"
#include <tracestream.h>
#include <regex>
#include <iostream>
#include <assert.h>
#include <boost/lexical_cast.hpp>

TraceHeader::TraceHeader(std::string tracefile)
{
    TraceStream* file = TraceStream::openFile(tracefile);
    if ( !file->good() )
        exit(-1);

    std::string header;
    file->getline(header);
    this->readHeader(header);
    file->close();
}

void TraceHeader::readHeader(std::string header)
{
    std::string shebang = "^#Paraver\\s*";

    std::string day_regex = "([0-2][0-9]|3[01])";
    std::string month_regex = "(0[1-9]|1[0-2])";
    std::string year_regex = "(19\\d{2}|20\\d{2})";
    std::string hour_regex = "([0-1][0-9]|2[0-3]):([0-5][0-9])";
    std::string date_regex = "\\(" + day_regex + "/" + month_regex 
        + "/" + year_regex + " at " + hour_regex + "\\)";

    std::string time_regex = "([0-9]+)_?(..)?";
    std::string reso_regex = "([0-9]+)(\\((.*)\\))?";

    std::string napps_regex = "([1-9]+)";
    std::string apps_map_regex = "([0-9]+)\\((.*)\\)";
    std::string ncomms_regex = ",([0-9]*)";
    std::string appl_regex = napps_regex 
        + ":" + apps_map_regex 
        + ncomms_regex;

    std::string header_regex = shebang 
        + date_regex
        + ":" + time_regex
        + ":" + reso_regex
        + ":" + appl_regex
        + ".*$";

    std::regex regex(header_regex);
    std::smatch m;

    int match = std::regex_match(header, m, regex);
    if (!match)
    {
        std::cout << "Error parsing header" << std::endl;
        exit(-1);
    }
    
    try
    {
        this->day = std::stoi(m[1]);
        this->month = std::stoi(m[2]);
        this->year = std::stoi(m[3]);
        this->hour = std::stoi(m[4]);
        this->minutes = std::stoi(m[5]);
        this->texe = boost::lexical_cast<long long>(m[6]);
        this->texe_units = m[7];
        this->nnodes = std::stoi(m[8]);
        this->napps = std::stoi(m[11]);
        this->ntasks = std::stoi(m[12]);
        this->mapping = m[13];
        this->ncomms = std::stoi(m[14]);
    } 
    catch (std::exception& e)
    {
        std::cout << "[TraceHeader] Exception on casting. Exiting..." << std::endl;
        exit(1);
    }


    assert(this->napps == 1);
}
