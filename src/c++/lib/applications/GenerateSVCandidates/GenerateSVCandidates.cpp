// -*- mode: c++; indent-tabs-mode: nil; -*-
//
// Manta
// Copyright (c) 2013 Illumina, Inc.
//
// This software is provided under the terms and conditions of the
// Illumina Open Source Software License 1.
//
// You should have received a copy of the Illumina Open Source
// Software License 1 along with this program. If not, see
// <https://github.com/downloads/sequencing/licenses/>.
//

///
/// \author Chris Saunders
///

#include "GenerateSVCandidates.hh"
#include "GSCOptions.hh"
#include "EdgeRetriever.hh"
#include "SVFinder.hh"
#include "WriteSVCandidatesToVcf.hh"

#include "common/OutStream.hh"
#include "manta/ReadGroupStatsSet.hh"

#include "boost/foreach.hpp"

#include <iostream>




static
void
runGSC(const GSCOptions& opt)
{
    {
        // early test that we have permission to write to output file
        OutStream outs(opt.candidateOutputFilename);

        if (! opt.somaticOutputFilename.empty())
        {
            OutStream somouts(opt.somaticOutputFilename);
        }
    }

    SVFinder finder(opt);

    // load in set:
    SVLocusSet set;
    set.load(opt.graphFilename.c_str());
    const SVLocusSet& cset(finder.getSet());

    EdgeRetriever edger(cset, opt.binCount, opt.binIndex);

    OutStream outs(opt.candidateOutputFilename);
    std::ostream& outfp(outs.getStream());
    writeSVCandidateVcfHeader(outfp);


    SVCandidateData svData;
    std::vector<SVCandidate> svs;
    while (edger.next())
    {
        const EdgeInfo& edge(edger.getEdge());

        // find number, type and breakend range of SVs on this edge:
        finder.findSVCandidates(edge,svData,svs);

        writeSVCandidatesToVcf(opt.referenceFilename, set,svData,svs,outfp);
    }

}



void
GenerateSVCandidates::
runInternal(int argc, char* argv[]) const
{

    GSCOptions opt;

    parseGSCOptions(*this,argc,argv,opt);
    runGSC(opt);
}