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


//SampleAnalyser headers
#include "SampleAnalyzer/Interfaces/fastjet/JetClusteringFastJet.h"
#include "SampleAnalyzer/Service/LoopService.h"

//FastJet headers
#include <fastjet/ClusterSequence.hh>
#include <fastjet/PseudoJet.hh>

using namespace MA5;


JetClusteringFastJet::JetClusteringFastJet(std::string Algo)
{ JetAlgorithm_=Algo; JetDefinition_=0; }

JetClusteringFastJet::~JetClusteringFastJet() 
{ if (JetDefinition_!=0) delete JetDefinition_; }


void JetClusteringFastJet::GetFinalState(const MCParticleFormat* part, std::set<const MCParticleFormat*>& finalstates)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (PHYSICS->Id->IsFinalState(part->daughters()[i])) finalstates.insert(part->daughters()[i]);
    else return GetFinalState(part->daughters()[i],finalstates);
  }
}

Bool_t JetClusteringFastJet::IsLast(const MCParticleFormat* part, EventFormat& myEvent)
{
  for (unsigned int i=0; i<part->daughters().size(); i++)
  {
    if (part->daughters()[i]->pdgid()==part->pdgid()) return false;
  }
  return true;
}


bool JetClusteringFastJet::Execute(SampleFormat& mySample, EventFormat& myEvent)
{
  if (mySample.mc()==0 || myEvent.mc()==0) return false;
  if (mySample.rec()==0) mySample.InitializeRec();
  if (myEvent.rec() ==0) myEvent.InitializeRec();

  // Reseting the reconstructed event
  myEvent.rec()->Reset();

  // Veto
  std::vector<bool> vetos(myEvent.mc()->particles().size(),false);
  std::set<const MCParticleFormat*> vetos2;

  // Filling the dataformat with electron/muon
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat& part = myEvent.mc()->particles()[i];
    UInt_t absid = std::abs(part.pdgid());

    // Rejecting particle with a null pt (initial state ?)
    if (part.pt()<1e-10) continue;

    // Treating intermediate particles
    if (PHYSICS->Id->IsInterState(part))
    {
      // rejecting not interesting particles
      if (absid!=5 && absid!=4 && absid!=15) continue;

      // keeping the last particle with the same id in the decay chain
      if (!IsLast(&part, myEvent)) continue;

      // looking for b quarks
      if (absid==5)
      {
        bool found=false;
        for (unsigned int j=0;j<myEvent.rec()->MCBquarks_.size();j++)
        {
          if (myEvent.rec()->MCBquarks_[j]==&(part)) 
          {found=true; break;}
        }
        if (!found) myEvent.rec()->MCBquarks_.push_back(&(part));
      }

      // looking for c quarks
      else if (absid==4)
      {
        bool found=false;
        for (unsigned int j=0;j<myEvent.rec()->MCCquarks_.size();j++)
        {
          if (myEvent.rec()->MCCquarks_[j]==&(part)) 
          {found=true; break;}
        }
        if (!found) myEvent.rec()->MCCquarks_.push_back(&(part));
      }

      // looking for taus
      else if (absid==15)
      {
        // rejecting particle if coming from hadronization
        if (LOOP->ComingFromHadronDecay(&part,mySample)) continue;

        // Looking taus daughters id
        bool leptonic   = true;
        bool muonic     = false;
        bool electronic = false;
        for (unsigned int j=0;j<part.daughters().size();j++)
        {
          UInt_t pdgid = std::abs(part.daughters()[j]->pdgid());
          if      (pdgid==13) muonic=true;
          else if (pdgid==11) electronic=true;
          else if (pdgid!=22 /*photons*/ &&
                   !(pdgid>=11 && pdgid<=16) /*neutrinos*/) 
                  leptonic=false;
        }
        if (!leptonic) {muonic=false; electronic=false;}

        // Saving taus decaying into muons (only one copy)
        if (muonic)
        {
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCMuonicTaus_.size();j++)
          {
            if (myEvent.rec()->MCMuonicTaus_[j]==&(part)) 
            {found=true; break;}
          }
          if (!found) myEvent.rec()->MCMuonicTaus_.push_back(&(part));
        }

        // Saving taus decaying into electrons (only one copy)
        else if (electronic)
        {
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCElectronicTaus_.size();j++)
          {
            if (myEvent.rec()->MCElectronicTaus_[j]==&(part)) 
            {found=true; break;}
          }
          if (!found) myEvent.rec()->MCElectronicTaus_.push_back(&(part));
        }

        // Saving taus decaying into hadrons (only copy)
        else
        {
          bool found=false;
          for (unsigned int j=0;j<myEvent.rec()->MCHadronicTaus_.size();j++)
          {
            if (myEvent.rec()->MCHadronicTaus_[j]==&(part)) 
            {found=true; break;}
          }
          if (!found) 
          {
            // Saving the hadrons in MC container
            myEvent.rec()->MCHadronicTaus_.push_back(&(part));

            // Applying efficiency
            if (!myTAUtagger_->IsIdentified()) continue;

            // Creating reco hadronic taus
            RecTauFormat* myTau = myEvent.rec()->GetNewTau();
            if (part.pdgid()>0) myTau->setCharge(-1);
            else myTau->setCharge(+1);
            myTau->setMomentum(part.momentum());
            myTau->setMc(&part);
            myTau->setDecayMode(PHYSICS->GetTauDecayMode(myTau->mc()));
            if (myTau->DecayMode()<=0) myTau->setNtracks(0); // ERROR case
            else if (myTau->DecayMode()==7 || 
                     myTau->DecayMode()==9) myTau->setNtracks(3); // 3-Prong
            else myTau->setNtracks(1); // 1-Prong

            // Searching final state
            GetFinalState(&part,vetos2);
          }
        }
      }
    }

    // Keeping only final states
    else if (PHYSICS->Id->IsFinalState(part))
    {
      // keeping only electron, muon and photon
      if (absid!=22 && absid!=11 && absid!=13) continue;

      // rejecting particle if coming from hadronization
      if (ExclusiveId_ && LOOP->ComingFromHadronDecay(&part,mySample)) continue;

      // Muons
      if (absid==13)
      {
        vetos[i]=true;
        RecLeptonFormat * muon = myEvent.rec()->GetNewMuon();
        muon->setMomentum(part.momentum());
        muon->setMc(&(part));
        if (part.pdgid()==13) muon->SetCharge(-1);
        else muon->SetCharge(+1);
      }

      // Electrons
      else if (absid==11)
      {
        vetos[i]=true;
        RecLeptonFormat * elec = myEvent.rec()->GetNewElectron();
        elec->setMomentum(part.momentum());
        elec->setMc(&(part));
        if (part.pdgid()==11) elec->SetCharge(-1);
        else elec->SetCharge(+1);
      }

      // Photons
      else if (absid==22)
      {
        if (LOOP->IrrelevantPhoton(&part,mySample)) continue;
        vetos[i]=true;
        RecPhotonFormat * photon = myEvent.rec()->GetNewPhoton();
        photon->setMomentum(part.momentum());
        photon->setMc(&(part));
      }
    }
  }

  double & TET = myEvent.rec()->TET();
  double & THT = myEvent.rec()->THT();

  // Preparing inputs
  std::vector<fastjet::PseudoJet> inputs;
  for (unsigned int i=0;i<myEvent.mc()->particles().size();i++)
  {
    const MCParticleFormat& part = myEvent.mc()->particles()[i];

    // Selecting input for jet clustering
    if (myEvent.mc()->particles()[i].statuscode()!=1)       continue;
    if (PHYSICS->Id->IsInvisible(myEvent.mc()->particles()[i])) continue;

    // ExclusiveId mode
    if (ExclusiveId_)
    {
      if (vetos[i]) continue;
      if (vetos2.find(&part)!=vetos2.end()) continue;
    }

    // NonExclusive Id mode
    else if (std::abs(myEvent.mc()->particles()[i].pdgid())==13) continue;

    // Filling good particle for clustering
    inputs.push_back(
          fastjet::PseudoJet ( myEvent.mc()->particles()[i].px(), 
                               myEvent.mc()->particles()[i].py(),
                               myEvent.mc()->particles()[i].pz(),
                               myEvent.mc()->particles()[i].e()   ));
    inputs.back().set_user_index(i);
  }

  // Clustering
  fastjet::ClusterSequence clust_seq(inputs, *JetDefinition_);

  // Getting jets with PTmin = 0
  std::vector<fastjet::PseudoJet> jets; 
  if (Exclusive_) jets = clust_seq.exclusive_jets(0.);
  else jets = clust_seq.inclusive_jets(0.);

  // Calculating the MET  
  ParticleBaseFormat* MET = myEvent.rec()->GetNewMet();
  ParticleBaseFormat* MHT = myEvent.rec()->GetNewMht();

  for (unsigned int i=0;i<jets.size();i++)
  {
    TLorentzVector q(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e());
    (*MET) -= q;
    (*MHT) -= q;
    THT += jets[i].pt();
    TET += jets[i].pt();
  }

  // Getting jets with PTmin
  if (Exclusive_) jets = clust_seq.exclusive_jets(Ptmin_);
  else jets = clust_seq.inclusive_jets(Ptmin_);

  // Filling the dataformat with jets
  for (unsigned int i=0;i<jets.size();i++)
  {
    RecJetFormat * jet = myEvent.rec()->GetNewJet();
    jet->setMomentum(TLorentzVector(jets[i].px(),jets[i].py(),jets[i].pz(),jets[i].e()));
    std::vector<fastjet::PseudoJet> constituents = clust_seq.constituents(jets[i]);
    UInt_t tracks = 0;
    for (unsigned int j=0;j<constituents.size();j++)
    {
      jet->AddConstituent(constituents[j].user_index());
      //      if (std::abs(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())==11) continue;
      if (PDG->IsCharged(myEvent.mc()->particles()[constituents[j].user_index()].pdgid())) tracks++;
    }
    jet->ntracks_ = tracks;
  }


  if (ExclusiveId_)
  {
    for (unsigned int i=0;i<myEvent.rec()->electrons().size();i++)
    {
      (*MET) -= myEvent.rec()->electrons()[i].momentum();
      TET += myEvent.rec()->electrons()[i].pt();
    }
    for (unsigned int i=0;i<myEvent.rec()->photons().size();i++)
    {
      (*MET) -= myEvent.rec()->photons()[i].momentum();
      TET += myEvent.rec()->photons()[i].pt();
    }
    for (unsigned int i=0;i<myEvent.rec()->taus().size();i++)
    {
      (*MET) -= myEvent.rec()->taus()[i].momentum();
      TET += myEvent.rec()->taus()[i].pt();
    }
  }

  for (unsigned int i=0;i<myEvent.rec()->muons().size();i++)
  {
    (*MET) -= myEvent.rec()->muons()[i].momentum();
    TET += myEvent.rec()->muons()[i].pt();
  }

  MET->momentum().SetPz(0.);
  MET->momentum().SetE(MET->momentum().Pt());
  MHT->momentum().SetPz(0.);
  MHT->momentum().SetE(MHT->momentum().Pt());

  myBtagger_->Execute(mySample,myEvent);
  myTAUtagger_->Execute(mySample,myEvent);

  return true;
}


