// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_H_
#define INTERFACE_ELECTRON_H_

#include <vector>
#include "TLorentzVector.h"

class Electron {
    friend class Electron_Factory;

   public:
    Electron(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Electron() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Int_t getCharge() { return Charge; }
    Int_t getConvVeto() { return ConvVeto; }
    Int_t getMissHits() { return MissHits; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Float_t getSCEn() { return SCEn; }
    Float_t getEcalEn() { return EcalEn; }
    Float_t getESEnP1() { return ESEnP1; }
    Float_t getESEnP2() { return ESEnP2; }
    Float_t getD0() { return D0; }
    Float_t getDz() { return Dz; }
    Float_t getPtError() { return PtError; }
    Float_t getSCEta() { return SCEta; }
    Float_t getSCPhi() { return SCPhi; }
    Float_t getSCRawEn() { return SCRawEn; }
    Float_t getESEffSigmaRR() { return ESEffSigmaRR; }
    Float_t getPFChIso() { return PFChIso; }
    Float_t getPFPhoIso() { return PFPhoIso; }
    Float_t getPFNeuIso() { return PFNeuIso; }
    Float_t getPFPUIso() { return PFPUIso; }
    Float_t getPFClusEcalIso() { return PFClusEcalIso; }
    Float_t getPFClusHcalIso() { return PFClusHcalIso; }
    Bool_t getIDMVAIso() { return IDMVAIso; }
    Bool_t getIDMVANoIso() { return IDMVANoIso; }
    ULong64_t getFiredSingleTrgs() { return FiredSingleTrgs; }
    ULong64_t getFiredDoubleTrgs() { return FiredDoubleTrgs; }
    ULong64_t getFiredL1Trgs() { return FiredL1Trgs; }
    Bool_t getID(int key) { return (IDbit >> key & 1) == 1; }

   private:
    TLorentzVector p4;
    Short_t IDbit;
    ULong64_t FiredSingleTrgs, FiredDoubleTrgs, FiredL1Trgs;
    Int_t Charge, ChargeConsistent, ConvVeto, MissHits, EcalDrivenSeed;
    Float_t En, SCEn, EcalEn, ESEnP1, ESEnP2, D0, Dz, Pt, PtError, Eta, Phi, SCEta, SCPhi,
        SCRawEn, ESEffSigmaRR, PFChIso, PFPhoIso, PFNeuIso, PFPUIso, PFClusEcalIso,
        PFClusHcalIso, IDMVAIso, IDMVANoIso;
};

Bool_t getIDMVAIso() {

}

#endif  // INTERFACE_ELECTRON_H_
