// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_H_
#define INTERFACE_ELECTRON_H_

#include <vector>
#include "./object.h"
#include "TLorentzVector.h"

class Electron {
    friend class Electron_Factory;

   public:
    Electron(Float_t _pt, Float_t _eta, Float_t _phi, Float_t _en) { this->p4.SetPtEtaPhiE(_pt, _eta, _phi, _en); }
    Electron() { this->p4.SetPtEtaPhiE(0, 0, 0, 0); }

    // getters
    TLorentzVector getP4() { return p4; }
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
    Float_t getR9() { return R9; }
    Float_t getCalibPt() { return CalibPt; }
    Float_t getCalibEn() { return CalibEn; }
    Float_t getSCEta() { return SCEta; }
    Float_t getSCPhi() { return SCPhi; }
    Float_t getSCRawEn() { return SCRawEn; }
    Float_t getSCEtaWidth() { return SCEtaWidth; }
    Float_t getSCPhiWidth() { return SCPhiWidth; }
    Float_t getHoverE() { return HoverE; }
    Float_t getEoverP() { return EoverP; }
    Float_t getEoverPout() { return EoverPout; }
    Float_t getEoverPInv() { return EoverPInv; }
    Float_t getBrem() { return Brem; }
    Float_t getdEtaAtVtx() { return dEtaAtVtx; }
    Float_t getdPhiAtVtx() { return dPhiAtVtx; }
    Float_t getSigmaIEtaIEtaFull5x5() { return SigmaIEtaIEtaFull5x5; }
    Float_t getSigmaIPhiIPhiFull5x5() { return SigmaIPhiIPhiFull5x5; }
    Float_t getConvVeto() { return ConvVeto; }
    Float_t getMissHits() { return MissHits; }
    Float_t getESEffSigmaRR() { return ESEffSigmaRR; }
    Float_t getPFChIso() { return PFChIso; }
    Float_t getPFPhoIso() { return PFPhoIso; }
    Float_t getPFNeuIso() { return PFNeuIso; }
    Float_t getPFPUIso() { return PFPUIso; }
    Float_t getPFClusEcalIso() { return PFClusEcalIso; }
    Float_t getPFClusHcalIso() { return PFClusHcalIso; }
    Float_t getIDMVAIso() { return IDMVAIso; }
    Float_t getIDMVANoIso() { return IDMVANoIso; }
    Float_t getR9Full5x5() { return R9Full5x5; }
    Float_t getEcalDrivenSeed() { return EcalDrivenSeed; }
    Float_t getTrkdxy() { return Trkdxy; }
    Float_t getKFHits() { return KFHits; }
    Float_t getKFChi2() { return KFChi2; }
    Float_t getGSFChi2() { return GSFChi2; }
    std::vector<Float_t> getGSFPt() { return GSFPt; }
    std::vector<Float_t> getGSFEta() { return GSFEta; }
    std::vector<Float_t> getGSFPhi() { return GSFPhi; }
    std::vector<Float_t> getGSFCharge() { return GSFCharge; }
    std::vector<Int_t> getGSFHits() { return GSFHits; }
    std::vector<Int_t> getGSFMissHits() { return GSFMissHits; }
    std::vector<Int_t> getGSFNHitsMax() { return GSFNHitsMax; }
    std::vector<Float_t> getGSFVtxProb() { return GSFVtxProb; }
    std::vector<Float_t> getGSFlxyPV() { return GSFlxyPV; }
    std::vector<Float_t> getGSFlxyBS() { return GSFlxyBS; }
    std::vector<Float_t> getBCEn() { return BCEn; }
    std::vector<Float_t> getBCEta() { return BCEta; }
    std::vector<Float_t> getBCPhi() { return BCPhi; }
    std::vector<Float_t> getBCS25() { return BCS25; }
    std::vector<Float_t> getBCS15() { return BCS15; }
    std::vector<Float_t> getBCSieie() { return BCSieie; }
    std::vector<Float_t> getBCSieip() { return BCSieip; }
    std::vector<Float_t> getBCSipip() { return BCSipip; }
    ULong64_t getFiredSingleTrgs() { return FiredSingleTrgs; }
    ULong64_t getFiredDoubleTrgs() { return FiredDoubleTrgs; }
    ULong64_t getFiredL1Trgs() { return FiredL1Trgs; }
    Short_t getIDbit() { return IDbit; }
    Float_t getScale_stat_up() { return Scale_stat_up; }
    Float_t getScale_stat_dn() { return Scale_stat_dn; }
    Float_t getScale_syst_up() { return Scale_syst_up; }
    Float_t getScale_syst_dn() { return Scale_syst_dn; }
    Float_t getScale_gain_up() { return Scale_gain_up; }
    Float_t getScale_gain_dn() { return Scale_gain_dn; }
    Float_t getResol_rho_up() { return Resol_rho_up; }
    Float_t getResol_rho_dn() { return Resol_rho_dn; }
    Float_t getResol_phi_up() { return Resol_phi_up; }
    Float_t getResol_phi_dn() { return Resol_phi_dn; }

   private:
    TLorentzVector p4;
    Short_t IDbit;
    ULong64_t FiredSingleTrgs, FiredDoubleTrgs, FiredL1Trgs;
    Int_t Charge, ChargeConsistent, ConvVeto, MissHits, EcalDrivenSeed;
    Float_t En, SCEn, EcalEn, ESEnP1, ESEnP2, D0, Dz, SIP, Pt, PtError, Eta, Phi, R9, CalibPt, CalibEn, SCEta, SCPhi,
        SCRawEn, SCEtaWidth, SCPhiWidth, HoverE, EoverP, EoverPout, EoverPInv, Brem, dEtaAtVtx, dPhiAtVtx,
        SigmaIEtaIEtaFull5x5, SigmaIPhiIPhiFull5x5, ESEffSigmaRR, PFChIso, PFPhoIso, PFNeuIso, PFPUIso, PFClusEcalIso,
        PFClusHcalIso, IDMVAIso, IDMVANoIso, R9Full5x5, Trkdxy, KFHits, KFChi2, GSFChi2, Scale_stat_up, Scale_stat_dn,
        Scale_syst_up, Scale_syst_dn, Scale_gain_up, Scale_gain_dn, Resol_rho_up, Resol_rho_dn, Resol_phi_up,
        Resol_phi_dn;
    std::vector<Int_t> GSFHits, GSFMissHits, GSFNHitsMax;
    std::vector<Float_t> GSFPt, GSFEta, GSFPhi, GSFCharge, GSFVtxProb, GSFlxyPV, GSFlxyBS, BCEn, BCEta, BCPhi, BCS25,
        BCS15, BCSieie, BCSieip, BCSipip;
};

#endif  // INTERFACE_ELECTRON_H_
