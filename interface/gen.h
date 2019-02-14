// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_H_
#define INTERFACE_GEN_H_

#include "TLorentzVector.h"

class Gen {
  friend class Gen_Factory;
 public:
  Gen(Float_t, Float_t, Float_t, Float_t);

  // setters
  inline void setPID(Int_t PID) { pid = PID; }

  // getters
  Int_t getPID() { return pid; }
  TLorentzVector getP4() { return p4; }

 private:
  Int_t pid;
  TLorentzVector p4;
};

Gen::Gen(Float_t pt, Float_t eta, Float_t phi, Float_t mass) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

#endif  // INTERFACE_GEN_H_
