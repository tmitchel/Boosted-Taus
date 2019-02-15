// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_H_
#define INTERFACE_JETS_H_

#include "TLorentzVector.h"

class Jets {
  friend class Jets_Factory;
 public:
  Jets(Float_t, Float_t, Float_t, Float_t);

  // getters
  TLorentzVector getP4() { return p4; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }

 private:
  TLorentzVector p4;
};

Jets::Jets(Float_t pt, Float_t eta, Float_t phi, Float_t energy) {
  p4.SetPtEtaPhiE(pt, eta, phi, energy);
}

#endif  // INTERFACE_JETS_H_
