// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_BOOSTED_H_
#define INTERFACE_BOOSTED_H_

#include "TLorentzVector.h"

class Boosted {
  friend class Boosted_Factory;
 public:
  Boosted(Float_t, Float_t, Float_t, Float_t);

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

Boosted::Boosted(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_BOOSTED_H_
