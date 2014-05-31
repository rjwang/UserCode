
/** \class EnergyScale
 *
 *  Applies energy scale.
 *
 *  $Date: 2013-11-21 11:38:22 +0100 (Thu, 21 Nov 2013) $
 *  $Revision: 1338 $
 *
 *
 *  \author P. Demin - UCL, Louvain-la-Neuve
 *
 */

#include "modules/EnergyScale.h"

#include "classes/DelphesClasses.h"
#include "classes/DelphesFactory.h"
#include "classes/DelphesFormula.h"

#include "ExRootAnalysis/ExRootResult.h"
#include "ExRootAnalysis/ExRootFilter.h"
#include "ExRootAnalysis/ExRootClassifier.h"

#include "TMath.h"
#include "TString.h"
#include "TFormula.h"
#include "TRandom3.h"
#include "TObjArray.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"

#include <algorithm> 
#include <stdexcept>
#include <iostream>
#include <sstream>

using namespace std;

//------------------------------------------------------------------------------

EnergyScale::EnergyScale() :
  fFormula(0), fItInputArray(0)
{
  fFormula = new DelphesFormula;
}

//------------------------------------------------------------------------------

EnergyScale::~EnergyScale()
{
  if(fFormula) delete fFormula;
}

//------------------------------------------------------------------------------

void EnergyScale::Init()
{
  // read resolution formula

  fFormula->Compile(GetString("ScaleFormula", "0.0"));

  // import input array

  fInputArray = ImportArray(GetString("InputArray", "FastJetFinder/jets"));
  fItInputArray = fInputArray->MakeIterator();

  // create output array

  fOutputArray = ExportArray(GetString("OutputArray", "jets"));
}

//------------------------------------------------------------------------------

void EnergyScale::Finish()
{
  if(fItInputArray) delete fItInputArray;
}

//------------------------------------------------------------------------------

void EnergyScale::Process()
{
  Candidate *candidate;
  TLorentzVector momentum;
  Double_t scale;
  
  fItInputArray->Reset();
  while((candidate = static_cast<Candidate*>(fItInputArray->Next())))
  {
    momentum = candidate->Momentum;

    scale = fFormula->Eval(momentum.Pt(), momentum.Eta());

    if(scale > 0.0) momentum *= scale;

    candidate = static_cast<Candidate*>(candidate->Clone());
    candidate->Momentum = momentum;

    fOutputArray->Add(candidate);
  }
}

//------------------------------------------------------------------------------
