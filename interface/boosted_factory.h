// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_BOOSTED_FACTORY_H_
#define INTERFACE_BOOSTED_FACTORY_H_

#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include "./boosted.h"
#include "TTree.h"

typedef std::vector<Boosted> VBoosted;

class Boosted_Factory {
 public:
  explicit Boosted_Factory(TTree *, std::string);
  void Run_Factory();

  // getters
  Int_t getNTotalBoosted() { return nBoostedTau; }
  Int_t getNGoodBoosted() { return nGoodTaus; }
  std::shared_ptr<VBoosted> getTaus() { return std::make_shared<VBoosted>(boosteds); }

 private:
  Int_t nBoostedTau, nGoodTaus;
  VBoosted boosteds;

  std::vector<Float_t> *boostedTauPt, *boostedTauEta, *boostedTauPhi, *boostedTauMass, *iso, *boostedTaudz, *boostedTaudxy, *boostedTauCharge;

  std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso, *pass_tight_iso, *pass_vtight_iso;

  std::vector<Bool_t> *boostedTaupfTausDiscriminationByDecayModeFinding, *boostedTaupfTausDiscriminationByDecayModeFindingNewDMs, *boostedTauByMVA6VLooseElectronRejection, *boostedTauByMVA6LooseElectronRejection,
      *boostedTauByMVA6MediumElectronRejection, *boostedTauByMVA6TightElectronRejection, *boostedTauByMVA6VTightElectronRejection, *boostedTauByLooseMuonRejection3, *boostedTauByTightMuonRejection3,
      *boostedTauByLooseCombinedIsolationDeltaBetaCorr3Hits, *boostedTauByMediumCombinedIsolationDeltaBetaCorr3Hits, *boostedTauByTightCombinedIsolationDeltaBetaCorr3Hits, *boostedTauLeadChargedHadronExists;

  std::vector<Int_t> *boostedTauDecayMode, *boostedTauNumSignalPFChargedHadrCands, *boostedTauNumSignalPFNeutrHadrCands, *boostedTauNumSignalPFGammaCands, *boostedTauNumSignalPFCands, *boostedTauNumIsolationPFChargedHadrCands,
      *boostedTauNumIsolationPFNeutrHadrCands, *boostedTauNumIsolationPFGammaCands, *boostedTauNumIsolationPFCands;
};

Boosted_Factory::Boosted_Factory(TTree *tree, std::string isoType = "IsolationMVArun2v2DBoldDMwLT")
    : boostedTauPt(nullptr),
      boostedTauEta(nullptr),
      boostedTauPhi(nullptr),
      boostedTauMass(nullptr),
      iso(nullptr),
      pass_vloose_iso(nullptr),
      pass_loose_iso(nullptr),
      pass_medium_iso(nullptr),
      pass_tight_iso(nullptr),
      pass_vtight_iso(nullptr),
      boostedTaupfTausDiscriminationByDecayModeFinding(nullptr),
      boostedTaupfTausDiscriminationByDecayModeFindingNewDMs(nullptr),
      boostedTauByMVA6VLooseElectronRejection(nullptr),
      boostedTauByMVA6LooseElectronRejection(nullptr),
      boostedTauByMVA6MediumElectronRejection(nullptr),
      boostedTauByMVA6TightElectronRejection(nullptr),
      boostedTauByMVA6VTightElectronRejection(nullptr),
      boostedTauByLooseMuonRejection3(nullptr),
      boostedTauByTightMuonRejection3(nullptr),
      boostedTauByLooseCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      boostedTauByMediumCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      boostedTauByTightCombinedIsolationDeltaBetaCorr3Hits(nullptr),
      boostedTauLeadChargedHadronExists(nullptr),
      boostedTauDecayMode(nullptr),
      boostedTauNumSignalPFChargedHadrCands(nullptr),
      boostedTauNumSignalPFNeutrHadrCands(nullptr),
      boostedTauNumSignalPFGammaCands(nullptr),
      boostedTauNumSignalPFCands(nullptr),
      boostedTauNumIsolationPFChargedHadrCands(nullptr),
      boostedTauNumIsolationPFNeutrHadrCands(nullptr),
      boostedTauNumIsolationPFGammaCands(nullptr),
      boostedTauNumIsolationPFCands(nullptr),
      boostedTauCharge(nullptr),
      boostedTaudz(nullptr),
      boostedTaudxy(nullptr) {
  tree->SetBranchAddress("nTau", &nBoostedTau);
  tree->SetBranchAddress("boostedTauPt", &boostedTauPt);
  tree->SetBranchAddress("boostedTauEta", &boostedTauEta);
  tree->SetBranchAddress("boostedTauPhi", &boostedTauPhi);
  tree->SetBranchAddress("boostedTauMass", &boostedTauMass);
  tree->SetBranchAddress("boostedTauByIsolationMVArun2v2DBoldDMwLTraw", &iso);
  tree->SetBranchAddress("boostedTauByVLooseIsolationMVArun2v2DBoldDMwLT", &pass_vloose_iso);
  tree->SetBranchAddress("boostedTauByLooseIsolationMVArun2v2DBoldDMwLT", &pass_loose_iso);
  tree->SetBranchAddress("boostedTauByMediumIsolationMVArun2v2DBoldDMwLT", &pass_medium_iso);
  tree->SetBranchAddress("boostedTauByTightIsolationMVArun2v2DBoldDMwLT", &pass_tight_iso);
  tree->SetBranchAddress("boostedTauByVTightIsolationMVArun2v2DBoldDMwLT", &pass_vtight_iso);
}

void Boosted_Factory::Run_Factory() {
  boosteds.clear();
  for (auto i = 0; i < nBoostedTau; i++) {
    auto boosted = Boosted(boostedTauPt->at(i), boostedTauEta->at(i), boostedTauPhi->at(i));
    if (boosted.getPt() > 20 && fabs(boosted.getEta()) < 2.3 && boosted.getIso(vloose)) {
      boosteds.push_back(boosted);
    }
  }

  // sort by pT
  std::sort(boosteds.begin(), boosteds.end(),
            [](Boosted& p1, Boosted& p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodTaus = boosteds.size();
}

#endif  // INTERFACE_BOOSTED_FACTORY_H_
