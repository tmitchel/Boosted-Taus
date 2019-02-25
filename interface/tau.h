// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TAU_H_
#define INTERFACE_TAU_H_

#include "TLorentzVector.h"

class Tau {
  friend class Tau_Factory;
 public:
  Tau(Float_t, Float_t, Float_t, Float_t);

  // getters
  TLorentzVector getP4() { return p4; }
  Bool_t geVLooseIso() { return pass_vloose_iso; }
  Bool_t getLooseIso() { return pass_loose_iso; }
  Bool_t getMediumIso() { return pass_medium_iso; }
  Bool_t getTightIso() { return pass_tight_iso; }
  Bool_t getVTightIso() { return pass_vtight_iso; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }

 private:
  TLorentzVector p4;
  Bool_t pass_vloose_iso, pass_loose_iso, pass_medium_iso, pass_tight_iso, pass_vtight_iso;
};

Tau::Tau(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_TAU_H_
