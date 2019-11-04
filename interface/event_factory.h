// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_EVENT_FACTORY_H_
#define INTERFACE_EVENT_FACTORY_H_

#include <vector>
#include "TLorentzVector.h"
#include "TTree.h"

// EventFactory combines all event level information
// and provides methods to access it. The EventFactory
// handles all the bit-shifts required to apply triggers.
class Event_Factory {
 public:
  explicit Event_Factory(TTree*);
  void Run_Factory() { MET.SetPtEtaPhiE(pfMET, 0, pfMETPhi, 0); }
  Bool_t getLepTrigger(int key) { return (HLTEleMuX >> key & 1) == 1; }
  Bool_t getPhoTrigger(int key) { return (HLTPho >> key & 1) == 1; }
  Bool_t getJetTrigger(int key) { return (HLTJet >> key & 1) == 1; }
  Bool_t getLepTriggerIsPre(int key) { return (HLTEleMuXIsPrescaled >> key & 1) == 1; }
  Bool_t getPhoTriggerIsPre(int key) { return (HLTPhoIsPrescaled >> key & 1) == 1; }
  Bool_t getJetTriggerIsPre(int key) { return (HLTJetIsPrescaled >> key & 1) == 1; }
  Int_t getRun() { return run; }
  Int_t getLumi() { return lumis; }
  Long64_t getEvent() { return evt; }
  TLorentzVector getMET() { return MET; }

 private:
  std::vector<int> *nPU, *puTrue;
  Long64_t evt;
  ULong64_t HLTEleMuX, HLTPho, HLTJet, HLTEleMuXIsPrescaled, HLTPhoIsPrescaled, HLTJetIsPrescaled;
  Int_t run, lumis;
  Float_t rho, pfMET, pfMETPhi;
  TLorentzVector MET;
};

// SetBranchAddresses when constructing an Event_Factory.
Event_Factory::Event_Factory(TTree* tree) {
  tree->SetBranchAddress("HLTEleMuX", &HLTEleMuX);
  tree->SetBranchAddress("HLTPho", &HLTPho);
  tree->SetBranchAddress("HLTJet", &HLTJet);
  tree->SetBranchAddress("HLTEleMuXIsPrescaled", &HLTEleMuXIsPrescaled);
  tree->SetBranchAddress("HLTPhoIsPrescaled", &HLTPhoIsPrescaled);
  tree->SetBranchAddress("HLTJetIsPrescaled", &HLTJetIsPrescaled);
  // tree->SetBranchAddress("nPU", &nPU);
  // tree->SetBranchAddress("puTrue", &puTrue);
  tree->SetBranchAddress("event", &evt);
  tree->SetBranchAddress("run", &run);
  tree->SetBranchAddress("lumis", &lumis);
  tree->SetBranchAddress("rho", &rho);
  tree->SetBranchAddress("pfMET", &pfMET);
  tree->SetBranchAddress("pfMETPhi", &pfMETPhi);
}

#endif  // INTERFACE_EVENT_FACTORY_H_
