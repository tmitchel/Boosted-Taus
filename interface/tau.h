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
  Bool_t getLooseIso() { return pass_loose_iso; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }

 private:
  TLorentzVector p4;
  Bool_t pass_loose_iso;
};

Tau::Tau(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_TAU_H_
