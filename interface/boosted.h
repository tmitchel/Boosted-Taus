// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_BOOSTED_H_
#define INTERFACE_BOOSTED_H_

#include <stdexcept>
#include "./util.h"
#include "TLorentzVector.h"

class Boosted {
  friend class Boosted_Factory;

 public:
  Boosted(Float_t, Float_t, Float_t, Float_t);

  // getters
  Bool_t getIso(working_point);
  Bool_t getDiscByDM(bool);
  Bool_t getEleRejection(working_point);
  Bool_t getMuRejection(working_point);
  Bool_t getCombinedIsolationDeltaBetaCorr3Hits(working_point);
  Bool_t getLeadChargedHadronExists() { return LeadChargedHadronExists; }
  Int_t getDecayMode() { return DecayMode; }
  Int_t getNumSignalPFChargedHadrCands() { return NumSignalPFChargedHadrCands; }
  Int_t getNumSignalPFNeutrHadrCands() { return NumSignalPFNeutrHadrCands; }
  Int_t getNumSignalPFGammaCands() { return NumSignalPFGammaCands; }
  Int_t getNumSignalPFCands() { return NumSignalPFCands; }
  Int_t getNumIsolationPFChargedHadrCands() { return NumIsolationPFChargedHadrCands; }
  Int_t getNumIsolationPFNeutrHadrCands() { return NumIsolationPFNeutrHadrCands; }
  Int_t getNumIsolationPFGammaCands() { return NumIsolationPFGammaCands; }
  Int_t getNumIsolationPFCands() { return NumIsolationPFCands; }
  Float_t getPt() { return p4.Pt(); }
  Float_t getEta() { return p4.Eta(); }
  Float_t getPhi() { return p4.Phi(); }
  Float_t getMass() { return p4.M(); }
  Float_t getCombinedIsolationDeltaBetaCorrRaw3Hits() { return CombinedIsolationDeltaBetaCorrRaw3Hits; }
  Float_t getCharge() { return Charge; }
  Float_t getVz() { return Vz; }
  Float_t getDxy() { return Dxy; }
  Float_t getZImpact() { return ZImpact; }
  Float_t getLeadChargedHadronEta() { return LeadChargedHadronEta; }
  Float_t getLeadChargedHadronPhi() { return LeadChargedHadronPhi; }
  Float_t getLeadChargedHadronPt() { return LeadChargedHadronPt; }
  Float_t getChargedIsoPtSum() { return ChargedIsoPtSum; }
  Float_t getNeutralIsoPtSum() { return NeutralIsoPtSum; }
  Float_t getPuCorrPtSum() { return PuCorrPtSum; }
  Float_t getfootprintCorrection() { return footprintCorrection; }
  Float_t getphotonPtSumOutsideSignalCone() { return photonPtSumOutsideSignalCone; }
  Float_t getDZ() { return dz; }
  Float_t getDXY() { return dxy; }
  TLorentzVector getP4() { return p4; }

 private:
  TLorentzVector p4;
  Bool_t pass_vloose_iso, pass_loose_iso, pass_medium_iso, pass_tight_iso, pass_vtight_iso;

  Bool_t pfTausDiscriminationByDecayModeFinding, pfTausDiscriminationByDecayModeFindingNewDMs, ByMVA6VLooseElectronRejection, ByMVA6LooseElectronRejection,
      ByMVA6MediumElectronRejection, ByMVA6TightElectronRejection, ByMVA6VTightElectronRejection, ByLooseMuonRejection3, ByTightMuonRejection3,
      ByLooseCombinedIsolationDeltaBetaCorr3Hits, ByMediumCombinedIsolationDeltaBetaCorr3Hits, ByTightCombinedIsolationDeltaBetaCorr3Hits, LeadChargedHadronExists;

  Int_t DecayMode, NumSignalPFChargedHadrCands, NumSignalPFNeutrHadrCands, NumSignalPFGammaCands, NumSignalPFCands, NumIsolationPFChargedHadrCands,
      NumIsolationPFNeutrHadrCands, NumIsolationPFGammaCands, NumIsolationPFCands;

  Float_t CombinedIsolationDeltaBetaCorrRaw3Hits, Charge, Vz, Dxy, Mass,
      ZImpact, LeadChargedHadronEta, LeadChargedHadronPhi, LeadChargedHadronPt, ChargedIsoPtSum, NeutralIsoPtSum, PuCorrPtSum, footprintCorrection,
      photonPtSumOutsideSignalCone, dz, dxy, Energy;
};

Boosted::Boosted(Float_t pt, Float_t eta, Float_t phi, Float_t mass = 1.77682) {
  p4.SetPtEtaPhiM(pt, eta, phi, mass);
}

Bool_t Boosted::getIso(working_point wp) {
  if (wp == 0) {
    return pass_vloose_iso;
  } else if (wp == 1) {
    return pass_loose_iso;
  } else if (wp == 2) {
    return pass_medium_iso;
  } else if (wp == 3) {
    return pass_tight_iso;
  } else if (wp == 4) {
    return pass_vtight_iso;
  }
  throw std::invalid_argument("Must use one of the provided tau isolation WPs");
}

Bool_t Boosted::getDiscByDM(bool newDM = false) {
  if (newDM) {
    return pfTausDiscriminationByDecayModeFindingNewDMs;
  }
  return pfTausDiscriminationByDecayModeFinding;
}

Bool_t Boosted::getEleRejection(working_point wp) {
  if (wp == 0) {
    return ByMVA6VLooseElectronRejection;
  } else if (wp == 1) {
    return ByMVA6LooseElectronRejection;
  } else if (wp == 2) {
    return ByMVA6MediumElectronRejection;
  } else if (wp == 3) {
    return ByMVA6TightElectronRejection;
  } else if (wp == 4) {
    return ByMVA6VTightElectronRejection;
  }
  throw std::invalid_argument("Must use one of the provided electron rejection WPs");
}

Bool_t Boosted::getMuRejection(working_point wp) {
  if (wp == 1) {
    return ByLooseMuonRejection3;
  } else if (wp == 3) {
    return ByTightMuonRejection3;
  }
  throw std::invalid_argument("Muon rejection working point was neither Tight nor Loose");
}

Bool_t Boosted::getCombinedIsolationDeltaBetaCorr3Hits(working_point wp) {
  if (wp == 1) {
    return ByLooseCombinedIsolationDeltaBetaCorr3Hits;
  } else if (wp == 2) {
    return ByMediumCombinedIsolationDeltaBetaCorr3Hits;
  } else if (wp == 3) {
    return ByTightCombinedIsolationDeltaBetaCorr3Hits;
  }
  throw std::invalid_argument("Must use one of the provided tau isolation WPs");

#endif  // INTERFACE_BOOSTED_H_
