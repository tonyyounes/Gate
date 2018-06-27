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

#include "GateConfiguration.h"

#include "GateVActor.hh"
#include "GateActorMessenger.hh"

#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>

#ifdef G4ANALYSIS_USE_ROOT

#ifndef GATETLEFLUENCEACTOR_HH
#define GATETLEFLUENCEACTOR_HH

class G4EmCalculator;

class GateTLEFluenceActor : public GateVActor
{
public:
  virtual ~GateTLEFluenceActor();

  // This macro initialize the CreatePrototype and CreateInstance
  FCT_FOR_AUTO_CREATOR_ACTOR(GateTLEFluenceActor)

  // Constructs the sensor
  virtual void Construct();

  // Callbacks
  virtual void BeginOfRunAction(const G4Run *r);
  virtual void BeginOfEventAction(const G4Event *);
  virtual void UserSteppingAction(const GateVVolume *, const G4Step *);
  virtual void EndOfEventAction(const G4Event *);

  /// Saves the data collected to the file
  virtual void SaveData();
  virtual void ResetData();

  virtual void Initialize(G4HCofThisEvent *) {}
  virtual void EndOfEvent(G4HCofThisEvent *) {}

  double GetEmin() { return mEmin; }
  double GetEmax() { return mEmax; }
  int GetENBins() { return mENBins; }

  void SetEmin(double v) { mEmin = v; }
  void SetEmax(double v) { mEmax = v; }
  void SetENBins(int v) { mENBins = v; }

  void SetELogScaleFlag(bool b) { mELogScaleFlag = b; }
  void SetSaveAsROOTFlag(bool b) { mSaveAsROOTFlag = b; }
  void SetSaveAsTextFlag(bool b) { mSaveAsTextFlag = b; }

protected:
  GateTLEFluenceActor(G4String name, G4int depth = 0);

  TFile *pTfile;
  TH1D *ElectronTLF;
  TH1D *PhotonTLF;
  TH1D *PrimaryElectronTLF;

  double mEmin;
  double mEmax;
  int mENBins;
  double *binEnergy;
  int nEvent;

  GateActorMessenger *pMessenger;

  G4String mHistName;
  bool mELogScaleFlag;
  bool mSaveAsROOTFlag;
  bool mSaveAsTextFlag;
  void SaveAsROOT();
  void SaveAsText(TH1D *histo, G4String initial_filename);

  G4EmCalculator *emcalc;
};

MAKE_AUTO_CREATOR_ACTOR(TLEFluenceActor, GateTLEFluenceActor)

#endif /* end #define GATETLEFLUENCEACTOR_HH */
#endif
