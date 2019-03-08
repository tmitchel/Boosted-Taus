// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TAU_FACTORY_H_
#define INTERFACE_TAU_FACTORY_H_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "./tau.h"
#include "TTree.h"

typedef std::vector<Tau> VTau;

class Tau_Factory {
 public:
  explicit Tau_Factory(TTree *, std::string);
  void Run_Factory();

  // getters
  Int_t getNTotalTau() { return nTau; }
  Int_t getNGoodTau() { return nGoodTau; }
  std::shared_ptr<VTau> getTaus() { return std::make_shared<VTau>(taus); }

 private:
  Int_t nTau, nGoodTau;
  VTau taus;

  std::vector<Bool_t> *taupfTausDiscriminationByDecayModeFinding, *taupfTausDiscriminationByDecayModeFindingNewDMs, *tauByMVA6VLooseElectronRejection, *tauByMVA6LooseElectronRejection,
      *tauByMVA6MediumElectronRejection, *tauByMVA6TightElectronRejection, *tauByMVA6VTightElectronRejection, *tauByLooseMuonRejection3, *tauByTightMuonRejection3,
      *tauByLooseCombinedIsolationDeltaBetaCorr3Hits, *tauByMediumCombinedIsolationDeltaBetaCorr3Hits, *tauByTightCombinedIsolationDeltaBetaCorr3Hits, *tauLeadChargedHadronExists;

  std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso, *pass_tight_iso, *pass_vtight_iso;

  std::vector<Int_t> *tauDecayMode, *tauNumSignalPFChargedHadrCands, *tauNumSignalPFNeutrHadrCands, *tauNumSignalPFGammaCands, *tauNumSignalPFCands, *tauNumIsolationPFChargedHadrCands,
      *tauNumIsolationPFNeutrHadrCands, *tauNumIsolationPFGammaCands, *tauNumIsolationPFCands;

  std::vector<Float_t> *tauCombinedIsolationDeltaBetaCorrRaw3Hits, *tauEta, *tauPhi, *tauPt, *tauEt, *tauCharge, *tauP, *tauPx, *tauPy, *tauPz, *tauVz, *tauEnergy, *tauMass, *tauDxy,
      *tauZImpact, *tauLeadChargedHadronEta, *tauLeadChargedHadronPhi, *tauLeadChargedHadronPt, *tauChargedIsoPtSum, *tauNeutralIsoPtSum, *tauPuCorrPtSum, *taufootprintCorrection,
      *tauphotonPtSumOutsideSignalCone, *taudz, *taudxy, *iso;
};

Tau_Factory::Tau_Factory(TTree *tree, std::string isoType = "IsolationMVArun2v2DBoldDMwLT")
    : tauPt(nullptr),
      tauEta(nullptr),
      tauPhi(nullptr),
      tauMass(nullptr),
      iso(nullptr),
      pass_vloose_iso(nullptr),
      pass_loose_iso(nullptr),
      pass_medium_iso(nullptr),
      pass_tight_iso(nullptr),
      pass_vtight_iso(nullptr),
      taupfTausDiscriminationByDecayModeFinding(nullptr),
      taupfTausDiscriminationByDecayModeFindingNewDMs(nullptr),
      tauByMVA6VLooseElectronRejection(nullptr),
      tauByMVA6LooseElectronRejection(nullptr),
      tauByMVA6MediumElectronRejection(nullptr),
      tauByMVA6TightElectronRejection(nullptr),
      tauByMVA6VTightElectronRejection(nullptr),
      tauByLooseMuonRejection3(nullptr),
      tauByTightMuonRejection3(nullptr),
      tauByLooseCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      tauByMediumCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      tauByTightCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      tauLeadChargedHadronExists(nullptr),
      tauDecayMode(nullptr),
      tauNumSignalPFChargedHadrCands(nullptr),
      tauNumSignalPFNeutrHadrCands(nullptr),
      tauNumSignalPFGammaCands(nullptr),
      tauNumSignalPFCands(nullptr),
      tauNumIsolationPFChargedHadrCands(nullptr),
      tauNumIsolationPFNeutrHadrCands(nullptr),
      tauNumIsolationPFGammaCands(nullptr),
      tauNumIsolationPFCands(nullptr),
      tauCombinedIsolationDeltaBetaCorrRaw3Hits(nullptr),
      tauCharge(nullptr),
      tauP(nullptr),
      tauPx(nullptr),
      tauPy(nullptr),
      tauPz(nullptr),
      tauVz(nullptr),
      tauEnergy(nullptr),
      tauDxy(nullptr),
      tauZImpact(nullptr),
      tauLeadChargedHadronEta(nullptr),
      tauLeadChargedHadronPhi(nullptr),
      tauLeadChargedHadronPt(nullptr),
      tauChargedIsoPtSum(nullptr),
      tauNeutralIsoPtSum(nullptr),
      tauPuCorrPtSum(nullptr),
      taufootprintCorrection(nullptr),
      tauphotonPtSumOutsideSignalCone(nullptr),
      taudz(nullptr),
      taudxy(nullptr) {
  tree->SetBranchAddress("nTau", &nTau);
  tree->SetBranchAddress("tauPt", &tauPt);
  tree->SetBranchAddress("tauEta", &tauEta);
  tree->SetBranchAddress("tauPhi", &tauPhi);
  tree->SetBranchAddress("tauMass", &tauMass);
  tree->SetBranchAddress(("tauBy"+isoType+"raw").c_str(), &iso);
  tree->SetBranchAddress(("tauByVLoose"+isoType).c_str(), &pass_vloose_iso);
  tree->SetBranchAddress(("tauByLoose"+isoType).c_str(), &pass_loose_iso);
  tree->SetBranchAddress(("tauByMedium"+isoType).c_str(), &pass_medium_iso);
  tree->SetBranchAddress(("tauByTight"+isoType).c_str(), &pass_tight_iso);
  tree->SetBranchAddress(("tauByVTight"+isoType).c_str(), &pass_vtight_iso);
  tree->SetBranchAddress("taupfTausDiscriminationByDecayModeFinding", &taupfTausDiscriminationByDecayModeFinding);
  tree->SetBranchAddress("taupfTausDiscriminationByDecayModeFindingNewDMs", &taupfTausDiscriminationByDecayModeFindingNewDMs);
  tree->SetBranchAddress("tauByMVA6VLooseElectronRejection", &tauByMVA6VLooseElectronRejection);
  tree->SetBranchAddress("tauByMVA6LooseElectronRejection", &tauByMVA6LooseElectronRejection);
  tree->SetBranchAddress("tauByMVA6MediumElectronRejection", &tauByMVA6MediumElectronRejection);
  tree->SetBranchAddress("tauByMVA6TightElectronRejection", &tauByMVA6TightElectronRejection);
  tree->SetBranchAddress("tauByMVA6VTightElectronRejection", &tauByMVA6VTightElectronRejection);
  tree->SetBranchAddress("tauByLooseMuonRejection3", &tauByLooseMuonRejection3);
  tree->SetBranchAddress("tauByTightMuonRejection3", &tauByTightMuonRejection3);
  tree->SetBranchAddress("tauByLooseCombinedIsolationDeltaBetaCorr3Hits", &tauByLooseCombinedIsolationDeltaBetaCorr3Hits);
  tree->SetBranchAddress("tauByMediumCombinedIsolationDeltaBetaCorr3Hits", &tauByMediumCombinedIsolationDeltaBetaCorr3Hits);
  tree->SetBranchAddress("tauByTightCombinedIsolationDeltaBetaCorr3Hits", &tauByTightCombinedIsolationDeltaBetaCorr3Hits);
  tree->SetBranchAddress("tauLeadChargedHadronExists", &tauLeadChargedHadronExists);
  tree->SetBranchAddress("tauDecayMode", &tauDecayMode);
  tree->SetBranchAddress("tauNumSignalPFChargedHadrCands", &tauNumSignalPFChargedHadrCands);
  tree->SetBranchAddress("tauNumSignalPFNeutrHadrCands", &tauNumSignalPFNeutrHadrCands);
  tree->SetBranchAddress("tauNumSignalPFGammaCands", &tauNumSignalPFGammaCands);
  tree->SetBranchAddress("tauNumSignalPFCands", &tauNumSignalPFCands);
  tree->SetBranchAddress("tauNumIsolationPFChargedHadrCands", &tauNumIsolationPFChargedHadrCands);
  tree->SetBranchAddress("tauNumIsolationPFNeutrHadrCands", &tauNumIsolationPFNeutrHadrCands);
  tree->SetBranchAddress("tauNumIsolationPFGammaCands", &tauNumIsolationPFGammaCands);
  tree->SetBranchAddress("tauNumIsolationPFCands", &tauNumIsolationPFCands);
  tree->SetBranchAddress("tauCombinedIsolationDeltaBetaCorrRaw3Hits", &tauCombinedIsolationDeltaBetaCorrRaw3Hits);
  tree->SetBranchAddress("tauCharge", &tauCharge);
  tree->SetBranchAddress("tauP", &tauP);
  tree->SetBranchAddress("tauPx", &tauPx);
  tree->SetBranchAddress("tauPy", &tauPy);
  tree->SetBranchAddress("tauPz", &tauPz);
  tree->SetBranchAddress("tauVz", &tauVz);
  tree->SetBranchAddress("tauEnergy", &tauEnergy);
  tree->SetBranchAddress("tauDxy", &tauDxy);
  tree->SetBranchAddress("tauZImpact", &tauZImpact);
  tree->SetBranchAddress("tauLeadChargedHadronEta", &tauLeadChargedHadronEta);
  tree->SetBranchAddress("tauLeadChargedHadronPhi", &tauLeadChargedHadronPhi);
  tree->SetBranchAddress("tauLeadChargedHadronPt", &tauLeadChargedHadronPt);
  tree->SetBranchAddress("tauChargedIsoPtSum", &tauChargedIsoPtSum);
  tree->SetBranchAddress("tauNeutralIsoPtSum", &tauNeutralIsoPtSum);
  tree->SetBranchAddress("tauPuCorrPtSum", &tauPuCorrPtSum);
  tree->SetBranchAddress("taufootprintCorrection", &taufootprintCorrection);
  tree->SetBranchAddress("tauphotonPtSumOutsideSignalCone", &tauphotonPtSumOutsideSignalCone);
  tree->SetBranchAddress("taudz", &taudz);
  tree->SetBranchAddress("taudxy", &taudxy);
}

void Tau_Factory::Run_Factory() {
  taus.clear();
  for (auto i = 0; i < nTau; i++) {
    // baseline/default selection
    if (tauPt->at(i) > 20 && tauEta->at(i) < 2.3 && pass_vloose_iso->at(i)) {
      auto tau = Tau(tauPt->at(i), tauEta->at(i), tauPhi->at(i), tauMass->at(i));  // build the tau

      tau.pass_vloose_iso = pass_vloose_iso->at(i);
      tau.pass_loose_iso = pass_loose_iso->at(i);
      tau.pass_medium_iso = pass_medium_iso->at(i);
      tau.pass_tight_iso = pass_tight_iso->at(i);
      tau.pass_vtight_iso = pass_vtight_iso->at(i);

      taus.push_back(tau);
    }
  }

  // sort by pT
  std::sort(taus.begin(), taus.end(), [](Tau &p1, Tau &p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodTau = taus.size();
}

#endif  // INTERFACE_TAU_FACTORY_H_
