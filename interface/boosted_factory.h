// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_BOOSTED_FACTORY_H_
#define INTERFACE_BOOSTED_FACTORY_H_

#include <memory>
#include <vector>
#include "./boosted.h"
#include "TTree.h"

typedef std::vector<Boosted> VBoosted;

class Boosted_Factory {
 public:
  explicit Boosted_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNBoosted() { return nBoostedTau; }
  std::shared_ptr<VBoosted> getTaus() { return std::make_shared<VBoosted>(boosteds); }

 private:
  Int_t nBoostedTau;
  VBoosted boosteds;
  std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso,
      *pass_tight_iso, *pass_vtight_iso;  // all are MVArun2v2DBoldDMwLT
  std::vector<Float_t> *pts, *etas, *phis, *masses, *iso;
};

Boosted_Factory::Boosted_Factory(TTree *tree) {
  tree->SetBranchAddress("nTau", &nBoostedTau);
  tree->SetBranchAddress("boostedTauPt", &pts);
  tree->SetBranchAddress("boostedTauEta", &etas);
  tree->SetBranchAddress("boostedTauPhi", &phis);
  tree->SetBranchAddress("boostedTauMass", &masses);
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
    auto boosted = Boosted(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    boosted.pass_loose_iso = pass_loose_iso->at(i);
    if (boosted.getPt() > 20 && fabs(boosted.getEta()) < 2.3 && boosted.getLooseIso()) {
      boosteds.push_back(boosted);
    }
  }
}

#endif  // INTERFACE_BOOSTED_FACTORY_H_
