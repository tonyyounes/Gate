/*----------------------
  Copyright (C): OpenGATE Collaboration

  This software is distributed under the terms
  of the GNU Lesser General  Public Licence (LGPL)
  See GATE/LICENSE.txt for further details
  ----------------------*/

/*
  \class  GateTLEFluenceActorMessenger
  \author tony.younes@inserm.fr
  \author maxime.chauvin@inserm.fr
*/

#include "GateConfiguration.h"
#ifdef G4ANALYSIS_USE_ROOT

#ifndef GATETLEFLUENCEACTORMESSENGER_HH
#define GATETLEFLUENCEACTORMESSENGER_HH

#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

#include "GateActorMessenger.hh"

class GateTLEFluenceActor;

class GateTLEFluenceActorMessenger : public GateActorMessenger
{
public:
  /// Constructor with pointer on the associated sensor
  GateTLEFluenceActorMessenger(GateTLEFluenceActor *v);
  virtual ~GateTLEFluenceActorMessenger();

  /// Command processing callback
  virtual void SetNewValue(G4UIcommand *, G4String);
  void BuildCommands(G4String base);

protected:
  /// Associated sensor
  GateTLEFluenceActor *pActor;

  /// Command objects
  G4UIcmdWithADoubleAndUnit *pEmaxCmd;
  G4UIcmdWithADoubleAndUnit *pEminCmd;
  G4UIcmdWithAnInteger *pNBinsCmd;
  G4UIcmdWithABool *pELogScaleCmd;
  G4UIcmdWithABool *pSaveAsROOTCmd;
  G4UIcmdWithABool *pSaveAsTextCmd;
};

#endif /* end #define GATETLEFLUENCEACTORMESSENGER_HH */
#endif
