// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_H_
#define INTERFACE_GEN_H_

#include "./object.h"
#include "TLorentzVector.h"

class Gen : public PObject {
    friend class Gen_Factory;

   public:
    Gen() : PObject() {}
    Gen(Float_t, Float_t, Float_t, Float_t, comp0);

    // getters
    Int_t getPID() const { return PID; }
    Int_t getMomPID() const { return MomPID; }
    Int_t getGMomPID() const { return GMomPID; }
    Int_t getStatus() const { return Status; }
    Int_t getParentage() const { return Parentage; }
    Float_t getVtx() const { return Vtx; }
    Float_t getVty() const { return Vty; }
    Float_t getVtz() const { return Vtz; }
    Float_t getEt() const { return Et; }
    Float_t getCalIso04() const { return CalIsoDR04; }
    Float_t getTrkIso03() const { return TrkIsoDR03; }
    Float_t getTrkIso04() const { return TrkIsoDR04; }
    Float_t getMomPt() const { return MomPt; }
    Float_t getMomEta() const { return MomEta; }
    Float_t getMomPhi() const { return MomPhi; }
    Float_t getMomMass() const { return MomMass; }
    TLorentzVector getMomP4();  // defined below

   private:
    TLorentzVector MomP4;
    Int_t PID, GMomPID, MomPID, Parentage, Status;
    Float_t Vtx, Vty, Vtz, E, Et, MomPt, MomMass, MomEta, MomPhi, CalIsoDR03, TrkIsoDR03, CalIsoDR04, TrkIsoDR04;
};

Gen::Gen(Float_t pt, Float_t eta, Float_t phi, Float_t energy, comp0 comp) : PObject(pt, eta, phi, energy, comp) {}

TLorentzVector Gen::getMomP4() {
    MomP4.SetPtEtaPhiM(MomPt, MomEta, MomPhi, MomMass);
    return MomP4;
}

#endif  // INTERFACE_GEN_H_
