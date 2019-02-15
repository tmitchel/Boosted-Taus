// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_H_
#define INTERFACE_GEN_H_

#include "TLorentzVector.h"

class Gen {
  friend class Gen_Factory;
 public:
  Gen(Float_t, Float_t, Float_t, Float_t);

  // getters
  Int_t getPID() { return pid; }
  TLorentzVector getP4() { return p4; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }

 private:
  Int_t pid;
  TLorentzVector p4;
};

Gen::Gen(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_GEN_H_
