// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_H_
#define INTERFACE_GEN_H_

#include "./object.h"
#include "TLorentzVector.h"

class Gen {
    friend class Gen_Factory;

   public:
    Gen(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Gen() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
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
    TLorentzVector p4, MomP4;
    Int_t PID, GMomPID, MomPID, Parentage, Status;
    Float_t Vtx, Vty, Vtz, E, Et, MomPt, MomMass, MomEta, MomPhi, CalIsoDR03, TrkIsoDR03, CalIsoDR04, TrkIsoDR04;
};

TLorentzVector Gen::getMomP4() {
    MomP4.SetPtEtaPhiM(MomPt, MomEta, MomPhi, MomMass);
    return MomP4;
}

#endif  // INTERFACE_GEN_H_
