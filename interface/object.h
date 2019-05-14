// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_OBJECT_H_
#define INTERFACE_OBJECT_H_

#include "TLorentzVector.h"

enum comp0 { energy, mass };

class PObject {
 public:
    PObject() { p4.SetPtEtaPhiE(0, 0, 0, 0); }
    PObject(Float_t, Float_t, Float_t, Float_t, comp0);
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getEnergy() { return p4.E(); }
    Float_t getMass() { return p4.M(); }

 protected:
    TLorentzVector p4;
};

PObject::PObject(Float_t pt_, Float_t eta_, Float_t phi_, Float_t e_, comp0 x0)  {
    if (x0 == 0) {
        p4.SetPtEtaPhiE(pt_, eta_, phi_, e_);
    } else if (x0 == 1) {
        p4.SetPtEtaPhiM(pt_, eta_, phi_, e_);
    }
}

#endif  // INTERFACE_OBJECT_H_
