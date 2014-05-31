////////////////////////////////////////////////////////////////////////////////
//  
//  Copyright (C) 2012-2013 Eric Conte, Benjamin Fuks
//  The MadAnalysis development team, email: <ma5team@iphc.cnrs.fr>
//  
//  This file is part of MadAnalysis 5.
//  Official website: <https://launchpad.net/madanalysis5>
//  
//  MadAnalysis 5 is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  MadAnalysis 5 is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with MadAnalysis 5. If not, see <http://www.gnu.org/licenses/>
//  
////////////////////////////////////////////////////////////////////////////////


// SampleAnalyzer headers
#include "SampleAnalyzer/JetClustering/JetClustererManager.h"
#ifdef FASTJET_USE
  #include "SampleAnalyzer/Interfaces/fastjet/JetClusteringStandard.h"
  #include "SampleAnalyzer/Interfaces/fastjet/JetClusteringSISCone.h"
  #include "SampleAnalyzer/Interfaces/fastjet/JetClusteringCDFMidpoint.h"
  #include "SampleAnalyzer/Interfaces/fastjet/JetClusteringCDFJetClu.h"
  #include "SampleAnalyzer/Interfaces/fastjet/JetClusteringGridJet.h"
#endif

using namespace MA5;

// -----------------------------------------------------------------------------
// BuildTable
// -----------------------------------------------------------------------------
void JetClustererManager::BuildTable()
{
  #ifdef FASTJET_USE
    Add("kt",          new JetClusteringStandard("kt"));
    Add("antikt",      new JetClusteringStandard("antikt"));
    Add("genkt",       new JetClusteringStandard("genkt"));
    Add("cambridge",   new JetClusteringStandard("cambridge"));
    Add("SISCone",     new JetClusteringSISCone());
    Add("CDFMidpoint", new JetClusteringCDFMidpoint());
    Add("CDFJetClu",   new JetClusteringCDFJetClu());
    Add("GridJet",     new JetClusteringGridJet());
  #endif
}

