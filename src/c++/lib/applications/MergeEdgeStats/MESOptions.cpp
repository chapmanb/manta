// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Manta - Structural Variant and Indel Caller
// Copyright (c) 2013-2015 Illumina, Inc.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

///
/// \author Chris Saunders
///

#include "MESOptions.hh"
#include "blt_util/log.hh"
#include "manta/ProgramUtil.hh"

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include <iostream>
#include <sstream>



static
void
usage(
    std::ostream& os,
    const manta::Program& prog,
    const boost::program_options::options_description& visible,
    const char* msg = nullptr)
{
    usage(os, prog, visible, "merge sv locus edge stats", "", msg);
}



void
parseMESOptions(
    const manta::Program& prog,
    int argc, char* argv[],
    MESOptions& opt)
{
    namespace po = boost::program_options;
    po::options_description req("configuration");
    req.add_options()
    ("stats-file", po::value(&opt.statsFilename),
     "input sv edge stats file (may be specified multiple times)")
    ("output-file", po::value(&opt.outputFilename),
     "merged output sv edge stats file (required)")
    ("report-file", po::value(&opt.reportFilename),
     "provide a summary report based on the merged edge stats");

    po::options_description help("help");
    help.add_options()
    ("help,h","print this message");

    po::options_description visible("options");
    visible.add(req).add(help);

    bool po_parse_fail(false);
    po::variables_map vm;
    try
    {
        po::store(po::parse_command_line(argc, argv, visible,
                                         po::command_line_style::unix_style ^ po::command_line_style::allow_short), vm);
        po::notify(vm);
    }
    catch (const boost::program_options::error& e)
    {
        // todo:: find out what is the more specific exception class thrown by program options
        log_os << "\nERROR: Exception thrown by option parser: " << e.what() << "\n";
        po_parse_fail=true;
    }

    if ((argc<=1) || (vm.count("help")) || po_parse_fail)
    {
        usage(log_os,prog,visible);
    }

    // fast check of config state:
    if (opt.statsFilename.empty())
    {
        usage(log_os,prog,visible, "Must specify at least 1 input sv edge stats file");
    }
    for (const std::string& statsFilename : opt.statsFilename)
    {
        if (! boost::filesystem::exists(statsFilename))
        {
            std::ostringstream oss;
            oss << "SV edge stats file does not exist: '" << statsFilename << "'";
            usage(log_os,prog,visible,oss.str().c_str());
        }
    }
    if (opt.outputFilename.empty())
    {
        usage(log_os,prog,visible, "Must specify sv edges stats output file");
    }
}

