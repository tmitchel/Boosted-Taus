// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_H_
#define INTERFACE_ELECTRON_H_

#include "TLorentzVector.h"

class Electron {
  friend class Electron_Factory;
 public:
  Electron(Float_t, Float_t, Float_t, Float_t);

  // getters
  TLorentzVector getP4() { return p4; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }

 private:
  TLorentzVector p4;
};

Electron::Electron(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_ELECTRON_H_
