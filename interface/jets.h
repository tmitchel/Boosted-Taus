// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_H_
#define INTERFACE_JETS_H_

#include "./object.h"
#include "TLorentzVector.h"

class Jets {
    friend class Jets_Factory;

   public:
    Jets(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Jets() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
    Float_t getPt() { return p4.Pt(); }
    Float_t getEta() { return p4.Eta(); }
    Float_t getPhi() { return p4.Phi(); }
    Float_t getMass() { return p4.M(); }
    Bool_t getJetPFLooseId() { return PFLooseId; }
    Int_t getLepTrackPID() { return LepTrackPID; }
    Int_t getPartonID() { return PartonID; }
    Int_t getHadFlvr() { return HadFlvr; }
    Int_t getID() { return ID; }
    Int_t getNCH() { return NCH; }
    Int_t getNNP() { return NNP; }
    ULong64_t getjetFiredTrgs() { return jetFiredTrgs; }
    Float_t getRawPt() { return RawPt; }
    Float_t getRawEn() { return RawEn; }
    Float_t getMt() { return Mt; }
    Float_t getArea() { return Area; }
    Float_t getLeadTrackPt() { return LeadTrackPt; }
    Float_t getLeadTrackEta() { return LeadTrackEta; }
    Float_t getLeadTrackPhi() { return LeadTrackPhi; }
    Float_t getLepTrackPt() { return LepTrackPt; }
    Float_t getLepTrackEta() { return LepTrackEta; }
    Float_t getLepTrackPhi() { return LepTrackPhi; }
    Float_t getCSV2BJetTags() { return CSV2BJetTags; }
    Float_t getDeepCSVTags_b() { return DeepCSVTags_b; }
    Float_t getDeepCSVTags_bb() { return DeepCSVTags_bb; }
    Float_t getDeepCSVTags_c() { return DeepCSVTags_c; }
    Float_t getDeepCSVTags_udsg() { return DeepCSVTags_udsg; }
    Float_t getP4Smear() { return P4Smear; }
    Float_t getP4SmearUp() { return P4SmearUp; }
    Float_t getP4SmearDo() { return P4SmearDo; }
    Float_t getPUID() { return PUID; }
    Float_t getJECUnc() { return JECUnc; }
    Float_t getFiredTrgs() { return FiredTrgs; }
    Float_t getCHF() { return CHF; }
    Float_t getNHF() { return NHF; }
    Float_t getCEF() { return CEF; }
    Float_t getNEF() { return NEF; }
    Float_t getMUF() { return MUF; }
    Float_t getVtxPt() { return VtxPt; }
    Float_t getVtxMass() { return VtxMass; }
    Float_t getVtxNtrks() { return VtxNtrks; }
    Float_t getVtx3DVal() { return Vtx3DVal; }
    Float_t getVtx3DSig() { return Vtx3DSig; }

   private:
   TLorentzVector p4;
    Bool_t PFLooseId;
    Int_t LepTrackPID, PartonID, HadFlvr, ID, PUFullID, NCH, NNP;
    ULong64_t jetFiredTrgs;
    Float_t RawPt, RawEn, Mt, Area, LeadTrackPt, LeadTrackEta, LeadTrackPhi, LepTrackPt, LepTrackEta, LepTrackPhi,
        CSV2BJetTags, DeepCSVTags_b, DeepCSVTags_bb, DeepCSVTags_c, DeepCSVTags_udsg, P4Smear, P4SmearUp, P4SmearDo,
        PUID, JECUnc, FiredTrgs, CHF, NHF, CEF, NEF, MUF, VtxPt, VtxMass, VtxNtrks, Vtx3DVal, Vtx3DSig;
};

#endif  // INTERFACE_JETS_H_
