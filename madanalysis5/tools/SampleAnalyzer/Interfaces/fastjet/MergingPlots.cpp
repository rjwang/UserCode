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


//SampleAnalyzer headers
#include "SampleAnalyzer/Interfaces/fastjet/MergingPlots.h"
#include "SampleAnalyzer/Core/Configuration.h"
#include "SampleAnalyzer/Service/Physics.h"
#include "SampleAnalyzer/Service/CompilationService.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>

//STL headers
#include <sstream>


using namespace MA5;

/*
extern"C"
{
  void ktclus_(int *imode, double PP[512][4], int* NN, double* ECUT, double Y[512]);
}
*/


bool MergingPlots::Initialize(const Configuration& cfg,
             const std::map<std::string,std::string>& parameters)
{
  // Options
  merging_njets_=0;
  merging_nqmatch_=4;
  merging_nosingrad_=false;

  // Reading options
  for (std::map<std::string,std::string>::const_iterator it=parameters.begin();
       it!=parameters.end();it++)
  {
    if (it->first=="njets")
    {
      std::stringstream str;
      str << it->second;
      str >> merging_njets_;
    }
    else
    {
      WARNING << "parameter '" << it->first 
              << "' is unknown and will be ignored." << endmsg;
    }
  }

  // Initializing DJR plots
  if (merging_njets_==0) 
  {
    ERROR << "number of jets requested for DJR plots is zero" << endmsg;
    return false;
  }
  DJR_.resize(merging_njets_);
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    std::stringstream str;
    str << "DJR" << i+1;
    std::string title;
    str >> title;
    DJR_[i].Initialize(DJR_.size()+1,title);
  }

  // Initializing clustering algorithm
  JetDefinition_ = new fastjet::JetDefinition(fastjet::kt_algorithm,1.0);
  return true;
}


void MergingPlots::Execute(SampleFormat& mySample, const EventFormat& myEvent)
{
  // Safety
  if (mySample.mc()==0) return;
  if (myEvent.mc()==0) return;

  // Getting number of additional jets in the event
  UInt_t njets = ExtractJetNumber(myEvent.mc(),mySample.mc());
  if (njets>DJR_.size()) return;

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  SelectParticles(inputs,myEvent.mc());

  // Getting DJR observables
  std::vector<Double_t> DJRvalues(DJR_.size(),0.);
  ExtractDJR(inputs,DJRvalues);


  // Getting results
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    /*     {    double djr = std::log10(sqrt(DJRvalues[i]));
            if (i==1 && njets==1 && djr > 1.8)
      {
        for (unsigned int j=0;j<myEvent->particles().size();j++)
        {
          bool accept=true;

          // Selecting partons (but not top quark)
          if (fabs(myEvent->particles()[j].pdgid())>5 && 
              myEvent->particles()[j].pdgid()!=21) accept=false;

          // Selecting final states
          if (myEvent->particles()[j].statuscode()!=2) accept=false;

          // Selecting states not coming from proton (beam remnant)
          if (myEvent->particles()[j].mothup1_==1) accept=false;
          if (myEvent->particles()[j].mothup1_==2) accept=false;

          const MCParticleFormat* myPart = &(myEvent->particles()[j]);
          bool test=true;
          while (myPart->mother1()!=0)
          {
            if (myPart->mothup1_==1 || myPart->mothup1_==2)
            { test=false; break;}
            else if (myPart->mothup1_<=6)
            { test=true; break;}
            else if (myPart->mother1()->pdgid()==91 || 
               myPart->mother1()->pdgid()==92)
           {test=false; break;}
            myPart = myPart->mother1();
          }
          if (!test) accept=false;

          if (fabs(myEvent->particles()[j].momentum().Rapidity())>5.0) accept=false;

          double PTJET=sqrt( myEvent->particles()[j].momentum().Px()*myEvent->particles()[j].momentum().Px() + 
                             myEvent->particles()[j].momentum().Py()*myEvent->particles()[j].momentum().Py() ); 
          double ETAJET=fabs(log(std::min((sqrt(PTJET*PTJET+myEvent->particles()[j].momentum().Pz()*myEvent->particles()[j].momentum().Pz())
                                    +fabs( myEvent->particles()[j].momentum().Pz() ))/PTJET,1e5)));


          std::cout << "i=" << j+1 
                    << " ; id=" << myEvent->particles()[j].pdgid()
                    << " ; s="  << myEvent->particles()[j].statuscode()
                    << " ; m1=" << myEvent->particles()[j].mothup1_ 
                    << " ; m2=" << myEvent->particles()[j].mothup2_
                    << " ; eta=" << ETAJET
                    << " ; rap=" << myEvent->particles()[j].momentum().Rapidity();

          if (accept) std::cout << " - ACCEPT";
          std::cout << std::endl;
        }
        //        exit(1);
      }
      }
    */
    double djr = 0;
    if (DJRvalues[i]>0) djr = std::log10(sqrt(DJRvalues[i]));
    DJR_[i].total->Fill(djr);
    DJR_[i].contribution[njets]->Fill(djr);
  }

}

void MergingPlots::Finalize(const SampleFormat& summary, const std::vector<SampleFormat>& files)
{
  // Saving plots into file
  Write_TextFormat(out());

  // Deleting plots
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    DJR_[i].Finalize();
  }
  DJR_.clear();

  // Free memory allocation
  if (JetDefinition_==0) delete JetDefinition_;
}




Double_t MergingPlots::rapidity(Double_t px, Double_t py, Double_t pz)
{
  double PTJET = sqrt( px*px + py*py);
  return fabs(log(std::min((sqrt(PTJET*PTJET+pz*pz)+fabs(pz ))/PTJET,1e5)));
}


void MergingPlots::ExtractDJRwithFortran(const std::vector<fastjet::PseudoJet>& inputs,std::vector<Double_t>& DJRvalues)
{
  double PP[512][4];
  UNUSED(PP);
  for (unsigned int i=0;i<inputs.size();i++)
  {
    PP[i][0]=inputs[i].px();
    PP[i][1]=inputs[i].py();
    PP[i][2]=inputs[i].pz();
    PP[i][3]=inputs[i].e();
  }
  //  int IMODE = 4313;
  //  int NN = inputs.size();
  //  if (NN>512) NN=512;
  //  double ECUT=1.;
  double Y[512];
  for (unsigned int i=0;i<512;i++) Y[i]=0;
  //if (NN!=0) ktclus_(&IMODE,PP,&NN,&ECUT,Y);

  for (unsigned int i=0;i<DJRvalues.size();i++)
  DJRvalues[i]=Y[i];
}

void MergingPlots::ExtractDJR(const std::vector<fastjet::PseudoJet>& inputs,std::vector<Double_t>& DJRvalues)
{
  // JetDefinition_
  fastjet::ClusterSequence sequence(inputs, *JetDefinition_);
  for (unsigned int i=0;i<DJRvalues.size();i++)
  DJRvalues[i]=sequence.exclusive_dmerge(i);
}


/// Saving merging plots in the text output file
void MergingPlots::Write_TextFormat(SAFWriter& output)
{
  *output.GetStream() << "<MergingPlots>" << std::endl;
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    DJR_[i].total->Write_TextFormat(output.GetStream());
    for (unsigned int j=0;j<DJR_[i].contribution.size();j++)
    {
      DJR_[i].contribution[j]->Write_TextFormat(output.GetStream());
    }
  }
  *output.GetStream() << "</MergingPlots>" << std::endl;
}


Bool_t MergingPlots::SavePlots(const std::string& filename)
{
  // Opening output file
  TFile* output = TFile::Open(filename.c_str(),"UPDATE");
  if (!output->IsOpen())
  {
    ERROR << "Output file called '" << filename 
          << "' is not found" << endmsg;
    return false;
  }

  // Writing ROOT file
  Write_RootFormat(output);
  return true;
}

/// Saving merging plots in the ROOT output file
void MergingPlots::Write_RootFormat(TFile* output)
{
  // Creating folder
  output->mkdir("merging");
  output->cd("merging");
  
  // Saving plots
  std::pair<TH1F*,TH1F*> histo;
  for (unsigned int i=0;i<DJR_.size();i++)
  {
    // Contribution
    for (unsigned int j=0;j<DJR_[i].contribution.size();j++)
    {
      std::stringstream str;
      str << "DJR" << i+1;
      str << "_" << j << "jet";
      std::string title;
      str >> title;
      histo.first  = new TH1F((title+"_pos").c_str(),"",100,0,100);
      histo.second = new TH1F((title+"_neg").c_str(),"",100,0,100);
      DJR_[i].contribution[j]->Write_RootFormat(histo);
      histo.first->Write();
      histo.second->Write();
    }

    // total
    std::stringstream str;
    str << "DJR" << i+1;
    str << "_total";
    std::string title;
    str >> title;
    histo.first  = new TH1F((title+"_pos").c_str(),"",100,0,100);
    histo.second = new TH1F((title+"_neg").c_str(),"",100,0,100);
    DJR_[i].total->Write_RootFormat(histo);
    histo.first->Write();
    histo.second->Write();
  }
  output->cd("");
}

/// Selecting particles for non-hadronized events
void MergingPlots::SelectParticles_NonHadronization(std::vector<fastjet::PseudoJet>& inputs, const MCEventFormat* myEvent)
{
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    // Selecting partons (but not top quark)
    if (fabs(myEvent->particles()[i].pdgid())>5 && 
        myEvent->particles()[i].pdgid()!=21) continue;

    // Selecting final states
    if (myEvent->particles()[i].statuscode()==3) continue;

    // Selecting states not coming from initial proton (beam remnant) 
    // or hadronization
    const MCParticleFormat* myPart = &(myEvent->particles()[i]);
    bool test=true;
    while (myPart->mother1()!=0)
    {
      if (myPart->mothup1_==1 || myPart->mothup1_==2)
      { test=false; break;}
      else if (myPart->mothup1_<=6)
      { test=true; break;}
      else if (myPart->mother1()->pdgid()==91 || 
               myPart->mother1()->pdgid()==92)
      {test=false; break;}
      myPart = myPart->mother1();
    }
    if (!test) continue;

    // Cut on the rapidity
    double ETAJET = rapidity(myEvent->particles()[i].momentum().Px(),
                             myEvent->particles()[i].momentum().Py(),
                             myEvent->particles()[i].momentum().Pz());
    if (fabs(ETAJET)>5) continue;
    
    // add the particle
    inputs.push_back(fastjet::PseudoJet ( myEvent->particles()[i].px(), 
                                          myEvent->particles()[i].py(), 
                                          myEvent->particles()[i].pz(), 
                                          myEvent->particles()[i].e() ) );

    // labeling the particle
    inputs.back().set_user_index(i);
  }
}


/// Selecting particles for non-hadronized events
void MergingPlots::SelectParticles(std::vector<fastjet::PseudoJet>& inputs,
                                    const MCEventFormat* myEvent)
{
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    // Selecting partons (but not top quark)
    if (fabs(myEvent->particles()[i].pdgid())>5 && 
        myEvent->particles()[i].pdgid()!=21) continue;

    // Selecting final states
    if (myEvent->particles()[i].statuscode()!=2) continue;

    // Selecting states not coming from initial proton (beam remnant) 
    // or hadronization
    const MCParticleFormat* myPart = &(myEvent->particles()[i]);
    bool test=true;
    while (myPart->mother1()!=0)
    {
      if (myPart->mothup1_==1 || myPart->mothup1_==2)
      { test=false; break;}
      else if (myPart->mothup1_<=6)
      { test=true; break;}
      else if (myPart->mother1()->pdgid()==91 || 
               myPart->mother1()->pdgid()==92)
      {test=false; break;}
      myPart = myPart->mother1();
    }
    if (!test) continue;

    // Cut on the rapidity
    double ETAJET = rapidity(myEvent->particles()[i].momentum().Px(),
                             myEvent->particles()[i].momentum().Py(),
                             myEvent->particles()[i].momentum().Pz());
    if (fabs(ETAJET)>5) continue;

    // Remove double counting
    if (myEvent->particles()[i].mother1()!=0 && myPart->mother2()==0)
    {
      if (myEvent->particles()[i].pdgid()==myEvent->particles()[i].mother1()->pdgid() &&
          myEvent->particles()[i].statuscode()==myEvent->particles()[i].mother1()->statuscode() &&
          fabs(myEvent->particles()[i].px()-myEvent->particles()[i].mother1()->px())<1e-04 &&
          fabs(myEvent->particles()[i].py()-myEvent->particles()[i].mother1()->py())<1e-04 &&
          fabs(myEvent->particles()[i].pz()-myEvent->particles()[i].mother1()->pz())<1e-04 )
        continue;
    }
    
    // add the particle
    inputs.push_back(fastjet::PseudoJet ( myEvent->particles()[i].px(), 
                                          myEvent->particles()[i].py(), 
                                          myEvent->particles()[i].pz(), 
                                          myEvent->particles()[i].e() ) );

    // labeling the particle
    inputs.back().set_user_index(i);
  }
}



/// Number of jets
UInt_t MergingPlots::ExtractJetNumber( const MCEventFormat* myEvent, 
                                      MCSampleFormat* mySample)
{
  UInt_t njets=0;
  for (unsigned int i=6;i<myEvent->particles().size();i++)
  {
    const MCParticleFormat* myPart = &myEvent->particles()[i];

    // keep particles generated during the matrix element calculation
    if (myPart->statuscode()!=3) continue;

    // keep only partons
    if (abs(myPart->pdgid())>merging_nqmatch_ && myPart->pdgid()!=21) continue;

    // keep only jets whose mother is one of the initial parton
    if (myPart->mother1()==0) continue;

    // coming from initial state ?
    if (myPart->mothup1_>6 && (myPart->mothup1_==0 || myPart->mothup2_==0)) continue;

    // removing color singlet
    /*
    if (merging_nosingrad_)
    {
      for (unsigned int j=0;j<myEvent->particles().size();j++)
      {
        if (i!=j) continue;

        const MCParticleFormat* myPart2 = &myEvent->particles()[j];
        
        // keep particles generated during the matrix element calculation
        if (myPart2->statuscode()!=3) continue;

        // keep only partons 
        if ( myPart2->pdgid()!=-myPart->pdgid() && 
             (myPart2->pdgid()!=21 && myPart->pdgid()!=21)) continue;

        // only final states

      }
    }
    */

    // count particle
    njets++;    
  }
  /*
  if (njets==3)  
  {
    for (unsigned int i=0;i<myEvent->particles().size();i++)
    {
      if (fabs(myEvent->particles()[i].pdgid())>5 && 
          myEvent->particles()[i].pdgid()!=21) continue;
      if (myEvent->particles()[i].statuscode()==1) continue;

      std::cout << "i=" << i+1 
                << " ; id=" << myEvent->particles()[i].pdgid()
                << " ; s="  << myEvent->particles()[i].statuscode()
                << " ; m1=" << myEvent->particles()[i].mothup1_ 
                << " ; m2=" << myEvent->particles()[i].mothup2_ << std::endl;
    }
    exit(1);
    }
  */
  return njets;  
}

