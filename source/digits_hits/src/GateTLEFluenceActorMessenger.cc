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

#include "GateTLEFluenceActorMessenger.hh"

#ifdef G4ANALYSIS_USE_ROOT

#include "GateTLEFluenceActor.hh"
#include "G4SystemOfUnits.hh"

//-----------------------------------------------------------------------------
GateTLEFluenceActorMessenger::GateTLEFluenceActorMessenger(GateTLEFluenceActor *v)
    : GateActorMessenger(v),
      pActor(v)
{
  BuildCommands(baseName + pActor->GetObjectName());
}

//-----------------------------------------------------------------------------
GateTLEFluenceActorMessenger::~GateTLEFluenceActorMessenger()
{
  delete pEmaxCmd;
  delete pEminCmd;
  delete pNBinsCmd;
  delete pELogScaleCmd;
  delete pSaveAsROOTCmd;
  delete pSaveAsTextCmd;
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActorMessenger::BuildCommands(G4String base)
{
  G4String guidance;
  G4String bb;

  bb = base + "/TLEFluence/setEmin";
  pEminCmd = new G4UIcmdWithADoubleAndUnit(bb, this);
  guidance = G4String("Set minimum energy ");
  pEminCmd->SetGuidance(guidance);
  pEminCmd->SetParameterName("Emin", false);
  pEminCmd->SetDefaultUnit("MeV");

  bb = base + "/TLEFluence/setEmax";
  pEmaxCmd = new G4UIcmdWithADoubleAndUnit(bb, this);
  guidance = G4String("Set maximum energy ");
  pEmaxCmd->SetGuidance(guidance);
  pEmaxCmd->SetParameterName("Emax", false);
  pEmaxCmd->SetDefaultUnit("MeV");

  bb = base + "/TLEFluence/setNumberOfBins";
  pNBinsCmd = new G4UIcmdWithAnInteger(bb, this);
  guidance = G4String("Set number of bins ");
  pNBinsCmd->SetGuidance(guidance);
  pNBinsCmd->SetParameterName("Nbins", false);

  bb = base+"/TLEFluence/setELogScale";
  pELogScaleCmd = new G4UIcmdWithABool(bb, this);
  guidance = G4String("Set the energy bin size to a log scale");
  pELogScaleCmd->SetGuidance(guidance);
  pELogScaleCmd->SetParameterName("ELogScale", false);

  bb = base + "/TLEFluence/saveAsROOT";
  pSaveAsROOTCmd = new G4UIcmdWithABool(bb, this);
  guidance = G4String("Save as .root file");
  pSaveAsROOTCmd->SetGuidance(guidance);  

  bb = base + "/TLEFluence/saveAsText";
  pSaveAsTextCmd = new G4UIcmdWithABool(bb, this);
  guidance = G4String("Save as .txt files");
  pSaveAsTextCmd->SetGuidance(guidance);  
}

//-----------------------------------------------------------------------------
void GateTLEFluenceActorMessenger::SetNewValue(G4UIcommand *cmd, G4String newValue)
{
  if (cmd == pEminCmd) pActor->SetEmin(pEminCmd->GetNewDoubleValue(newValue));
  if (cmd == pEmaxCmd) pActor->SetEmax(pEmaxCmd->GetNewDoubleValue(newValue));
  if (cmd == pNBinsCmd) pActor->SetENBins(pNBinsCmd->GetNewIntValue(newValue));
  if(cmd == pELogScaleCmd) pActor->SetELogScaleFlag(pELogScaleCmd->GetNewBoolValue(newValue));
  if(cmd == pSaveAsROOTCmd) pActor->SetSaveAsROOTFlag(pSaveAsROOTCmd->GetNewBoolValue(newValue));
  if(cmd == pSaveAsTextCmd) pActor->SetSaveAsTextFlag(pSaveAsTextCmd->GetNewBoolValue(newValue));
  GateActorMessenger::SetNewValue(cmd, newValue);
}

#endif
