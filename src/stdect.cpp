/*
 * stdect.cpp
 * Copyright (C) 2018 Juan Francisco Martínez Vera <juan.martinez[AT]bsc.es>
 *
 * Distributed under terms of the MIT license.
 */

#include <iostream>

// Some utils
#include <UIParaverTraceConfig.h>
#include <TraceHeader.h>

// Include workflow phases
#include <pipeline.h>
#include <nptrace.h>
#include <reducer.h>
#include <loops_identification.h>
#include <loops_merge.h>
#include <pseudocode.h>

// Boost helpers
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/program_options.hpp>
#include <boost/program_options/positional_options.hpp>

namespace po = boost::program_options;

int main(int argc, char **argv)
{
    std::string log_severity;
    std::vector<std::string> log_severity_vals = {
            "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
    };
    std::string tracefile;
    float filter_lbound;
    double eps;
    size_t minPts;
    double eps_tl;
    size_t minPts_tl;

    po::options_description general("Options");
    general.add_options()
        ("help,h", "Show this help message")
        ("trace,t", po::value<std::string>(&tracefile)->required(),
         "Trace to analyze")
        ("log,l", po::value<std::string>(&log_severity)->default_value("INFO"),
         "Log messages level.")
    ;

    po::options_description reduction("Reduction");
    reduction.add_options()
        ("bottom-bound", po::value<float>(&filter_lbound)->default_value(0.01), 
         "All unique mpi calls explainig less than this percentage (0 to 1) "\
         "will be filtered.")
    ;

    po::options_description loops_clustering("Loops clustering");
    loops_clustering.add_options()
        ("eps", po::value<double>(&eps)->default_value(0.1),
         "Epsilon used in DBSCAN algorithm for loops clustering.")
        ("minPts", po::value<size_t>(&minPts)->default_value(1),
         "minPts used in DBSCAN algorithm for loops clustering.")
    ;

    po::options_description loops_merge_opts("Loops merge");
    loops_merge_opts.add_options()
        ("eps-tl", po::value<double>(&eps_tl)->default_value(0.1),
         "Epsilon used in DBSCAN algorithm for top level loops clustering.")
        ("minPts-tl", po::value<size_t>(&minPts_tl)->default_value(1),
         "minPts used in DBSCAN algorithm for top level loops clustering.")
    ;


    po::positional_options_description pd;
    pd.add("trace",1);

    po::options_description all("Allowed options");
    all.add(general)
       .add(reduction)
       .add(loops_clustering)
       .add(loops_merge_opts);

    po::variables_map varmap;
    try
    {
        po::store(po::command_line_parser(argc,argv)
                .options(all)
                .positional(pd).run(), varmap);
        po::notify(varmap);
    }
    catch(po::required_option& e)
    {
        BOOST_LOG_TRIVIAL(fatal) << "Error parsing arguments";
        BOOST_LOG_TRIVIAL(fatal) << e.what();
        exit(-1);
    }
    catch(...)
    {
        BOOST_LOG_TRIVIAL(fatal) << "Revise arguments!";
        exit(-1);
    }
    if (varmap.count("help"))
    {
        std::cout << all << std::endl;
        exit(EXIT_FAILURE);
    }


    auto log_severity_level_it = std::find(
            log_severity_vals.begin(),
            log_severity_vals.end(), log_severity);
    
    if (log_severity_level_it == log_severity_vals.end())
    {
        BOOST_LOG_TRIVIAL(fatal) << "Selected log severity level does not exist.";
        exit(-1);
    }
    auto log_severity_level = std::distance(
            log_severity_vals.begin(), 
            log_severity_level_it);

    boost::log::core::get()->set_filter
    (
        boost::log::trivial::severity >= log_severity_level
    );
    
    std::string paraver_pcf;
    size_t start_pos = tracefile.find(".prv");
    if (start_pos != std::string::npos)
    {
        paraver_pcf = tracefile.substr(0, start_pos);
        paraver_pcf += ".pcf";
    }
    else
    {
        BOOST_LOG_TRIVIAL(fatal) << "Invalid trace name." << std::endl;
        exit(-1);
    }

    libparaver::UIParaverTraceConfig trace_semantic;
    trace_semantic.parse(paraver_pcf);

    TraceHeader trace_info(tracefile);

    // Declare all phases
    NPTrace parser;
    Reducer reducer(trace_info.texe, filter_lbound);
    LoopsIdentification loops_id(eps, minPts);
    LoopsMerge loops_merge(eps_tl, minPts_tl);
    Pseudocode pseudocode(&trace_semantic);

    // Connect them
    parser.connect(&reducer);
    reducer.connect(&loops_id);
    loops_id.connect(&loops_merge);
    loops_merge.connect(&pseudocode);

    // Just run the first stage
    parser.setInput(&tracefile);
    parser.run();
}
