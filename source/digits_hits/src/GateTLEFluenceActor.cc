/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

/*
  \class  GateTLEFluenceActor
  \author tony.younes@inserm.fr
  \author maxime.chauvin@inserm.fr
*/

#include "GateTLEFluenceActor.hh"
#include "GateTLEFluenceActorMessenger.hh"

#include "GateMiscFunctions.hh"

#ifdef G4ANALYSIS_USE_ROOT
#include <G4EmCalculator.hh>

//-----------------------------------------------------------------------------
GateTLEFluenceActor::GateTLEFluenceActor(G4String name, G4int depth) : GateVActor(name, depth)
{
  GateDebugMessageInc("Actor", 4, "GateTLEFluenceActor() -- begin\n");

  mEmin = GetEmin();
  mEmax = GetEmax();
  mENBins = GetENBins();

  mELogScaleFlag = true;
  mSaveAsROOTFlag = true;
  mSaveAsTextFlag = false;

  emcalc = new G4EmCalculator;

  pMessenger = new GateTLEFluenceActorMessenger(this);
  GateDebugMessageDec("Actor", 4, "GateTLEFluenceActor() -- end\n");
}

//-----------------------------------------------------------------------------
GateTLEFluenceActor::~GateTLEFluenceActor()
{
  delete emcalc;
  delete pMessenger;
  delete[] binEnergy;
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::Construct()
{
  GateVActor::Construct();
  EnableBeginOfRunAction(true);
  EnableBeginOfEventAction(true);
  EnableUserSteppingAction(true);
  EnableEndOfEventAction(true);

  binEnergy = new double[mENBins + 1];
  if (mELogScaleFlag)
  {
    double logIncrement = (log(mEmax) - log(mEmin)) / mENBins;
    for (int i = 0; i < mENBins + 1; i++)
    {
      binEnergy[i] = exp(log(mEmin) + i * logIncrement);
    }
  }
  else
  {
    double increment = (mEmax - mEmin) / mENBins;
    for (int i = 0; i < mENBins + 1; i++)
    {
      binEnergy[i] = mEmin + i * increment;
    }
  }

  if (mSaveAsROOTFlag) pTfile = new TFile(mSaveFilename, "RECREATE");

  ElectronTLF = new TH1D("ElectronTrackLengthFluence", "Energy Fluence", mENBins, binEnergy);
  ElectronTLF->SetXTitle("Kinetic Energy (MeV)");

  PrimaryElectronTLF = new TH1D("PrimaryElectronTrackLengthFluence", "Energy Fluence", mENBins, binEnergy);
  PrimaryElectronTLF->SetXTitle("Kinetic Energy (MeV)");

  PhotonTLF = new TH1D("PhotonTrackLength", "PhotonFluence", mENBins, binEnergy);
  PhotonTLF->SetXTitle("Kinetic Energy (MeV)");
  ResetData();
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::SaveData()
{
  // divide fluence by the bin width
  ElectronTLF->Scale(1, "width");
  PhotonTLF->Scale(1, "width");
  PrimaryElectronTLF->Scale(1, "width");

  if (mSaveAsROOTFlag)
  {
    SaveAsROOT();
  }

  if (mSaveAsTextFlag)
  {
    SaveAsText(ElectronTLF, mSaveFilename);
    SaveAsText(PrimaryElectronTLF, mSaveFilename);
    SaveAsText(PhotonTLF, mSaveFilename);
  }
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::ResetData()
{
  ElectronTLF->Reset();
  PrimaryElectronTLF->Reset();
  PhotonTLF->Reset();
  nEvent = 0;
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::BeginOfRunAction(const G4Run *)
{
  GateDebugMessage("Actor", 3, "GateTLEFluenceActor -- Begin of Run\n");
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::BeginOfEventAction(const G4Event *)
{
  GateDebugMessage("Actor", 3, "GateTLEFluenceActor -- Begin of Event\n");
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::EndOfEventAction(const G4Event *)
{
  GateDebugMessage("Actor", 3, "GateTLEFluenceActor -- End of Event\n");
  nEvent++;
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::UserSteppingAction(const GateVVolume *, const G4Step *step)
{
  double Ef = step->GetPostStepPoint()->GetKineticEnergy();
  double Ei = step->GetPreStepPoint()->GetKineticEnergy();
  if (Ei > mEmax) GateError("Particle energy higher than Emax - abort simulation");
  double Weight = step->GetTrack()->GetWeight();

  ////////////////////Total Photons///////////////////////////////
  G4String PartName = step->GetTrack()->GetDefinition()->GetParticleName();
  if (PartName == "gamma")
  {
    double sumG = step->GetStepLength();
    double FluenceG = sumG * Weight;
    PhotonTLF->Fill(Ei / MeV, FluenceG);
  }
  ////////////////////Total Electron+positrons///////////////////////////////
  else if (PartName == "e-" || PartName == "e+")
  {
    int parent_id = step->GetTrack()->GetParentID();
    G4String material = step->GetPreStepPoint()->GetMaterial()->GetName();
    double cut = 1;

    int i = 0;
    while (binEnergy[i] < Ef) i++;
    int x = i;
    double Elow = binEnergy[x];

    while (binEnergy[i] < Ei) i++;
    int y = i - 1;
    double Ehigh = binEnergy[y];

    if (y < x) // The step is totaly in the bin i
    {
      double Emean = (Ei + Ef) / 2;
      double DEDXf = emcalc->ComputeElectronicDEDX(Ei, PartName, material, cut);
      double sumf = (Ei - Ef) / DEDXf;
      double Fluence1 = sumf * Weight;
      ElectronTLF->Fill(Emean / MeV, Fluence1);
      if (parent_id == 1)
      {
        PrimaryElectronTLF->Fill(Emean / MeV, Fluence1);
      }
    }

    else if (y == x) //step is distributed in two bins
    {
      double Emidlow = (Elow + Ef) / 2;
      double DEDXf = emcalc->ComputeElectronicDEDX(Elow, PartName, material, cut);
      double sumf = (Elow - Ef) / DEDXf;
      double Fluencef = sumf * Weight;
      ElectronTLF->Fill(Emidlow / MeV, Fluencef);
      if (parent_id == 1)
      {
        PrimaryElectronTLF->Fill(Emidlow / MeV, Fluencef);
      }

      double Emidhigh = (Ei + Ehigh) / 2;
      double DEDXi = emcalc->ComputeElectronicDEDX(Ei, PartName, material, cut);
      double sumi = (Ei - Ehigh) / DEDXi;
      double Fluencei = sumi * Weight;
      ElectronTLF->Fill(Emidhigh / MeV, Fluencei);
      if (parent_id == 1)
      {
        PrimaryElectronTLF->Fill(Emidhigh / MeV, Fluencei);
      }
    }

    else //step is distributed in different bins
    {
      double Emidlow = (Elow + Ef) / 2;
      double DEDXf = emcalc->ComputeElectronicDEDX(Elow, PartName, material, cut);
      double sumf = (Elow - Ef) / DEDXf;
      double Fluencef = sumf * Weight;
      ElectronTLF->Fill(Emidlow / MeV, Fluencef);
      if (parent_id == 1)
      {
        PrimaryElectronTLF->Fill(Emidlow / MeV, Fluencef);
      }

      for (i = x; i < y; i++)
      {
        double Emid = (binEnergy[i + 1] + binEnergy[i]) / 2;
        double DEDXmid = emcalc->ComputeElectronicDEDX(binEnergy[i + 1], PartName, material, cut);
        double summid = (binEnergy[i + 1] - binEnergy[i]) / DEDXmid;
        double Fluencemid = summid * Weight;
        ElectronTLF->Fill(Emid / MeV, Fluencemid);
        if (parent_id == 1)
        {
          PrimaryElectronTLF->Fill(Emid / MeV, Fluencemid);
        }
      }

      double Emidhigh = (Ei + Ehigh) / 2;
      double DEDXi = emcalc->ComputeElectronicDEDX(Ei, PartName, material, cut);
      double sumi = (Ei - Ehigh) / DEDXi;
      double Fluencei = sumi * Weight;
      ElectronTLF->Fill(Emidhigh / MeV, Fluencei);
      if (parent_id == 1)
      {
        PrimaryElectronTLF->Fill(Emidhigh / MeV, Fluencei);
      }
    }
  }
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::SaveAsROOT()
{
  pTfile->Write();
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActor::SaveAsText(TH1D *histo, G4String initial_filename)
{
  std::string filename = removeExtension(initial_filename);
  filename = filename + "_" + histo->GetName() + ".txt";

  std::ofstream oss;
  OpenFileOutput(filename, oss);

  oss << "Number of bins = " << histo->GetNbinsX() << std::endl
      << "Number of events: " << nEvent << std::endl
      << " Energy Fluence differential in energy" << std::endl;
  for (int i = 1; i <= histo->GetNbinsX() + 1; i++)
  {
    oss << histo->GetBinLowEdge(i) << " " << histo->GetBinContent(i) << std::endl;
  }
  oss.close();
}

#endif
