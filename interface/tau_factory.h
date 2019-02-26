// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TAU_FACTORY_H_
#define INTERFACE_TAU_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./tau.h"
#include "TTree.h"

typedef std::vector<Tau> VTau;

class Tau_Factory {
 public:
  explicit Tau_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalTau() { return nTau; }
  Int_t getNGoodTau() { return nGoodTau; }
  std::shared_ptr<VTau> getTaus() { return std::make_shared<VTau>(taus); }

 private:
  Int_t nTau, nGoodTau;
  VTau taus;
  std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso, *pass_tight_iso,
      *pass_vtight_iso;  // all are MVArun2v2DBoldDMwLT
  std::vector<Float_t> *pts, *etas, *phis, *masses, *iso;
};

Tau_Factory::Tau_Factory(TTree *tree)
    : pts(nullptr),
      etas(nullptr),
      phis(nullptr),
      masses(nullptr),
      iso(nullptr),
      pass_vloose_iso(nullptr),
      pass_loose_iso(nullptr),
      pass_medium_iso(nullptr),
      pass_tight_iso(nullptr),
      pass_vtight_iso(nullptr) {
  tree->SetBranchAddress("nTau", &nTau);
  tree->SetBranchAddress("tauPt", &pts);
  tree->SetBranchAddress("tauEta", &etas);
  tree->SetBranchAddress("tauPhi", &phis);
  tree->SetBranchAddress("tauMass", &masses);
  tree->SetBranchAddress("tauByIsolationMVArun2v2DBoldDMwLTraw", &iso);
  tree->SetBranchAddress("tauByVLooseIsolationMVArun2v2DBoldDMwLT", &pass_vloose_iso);
  tree->SetBranchAddress("tauByLooseIsolationMVArun2v2DBoldDMwLT", &pass_loose_iso);
  tree->SetBranchAddress("tauByMediumIsolationMVArun2v2DBoldDMwLT", &pass_medium_iso);
  tree->SetBranchAddress("tauByTightIsolationMVArun2v2DBoldDMwLT", &pass_tight_iso);
  tree->SetBranchAddress("tauByVTightIsolationMVArun2v2DBoldDMwLT", &pass_vtight_iso);
}

void Tau_Factory::Run_Factory() {
  taus.clear();
  for (auto i = 0; i < nTau; i++) {
    // baseline/default selection
    if (pts->at(i) > 20 && etas->at(i) < 2.3 && pass_vloose_iso->at(i)) {
      auto tau = Tau(pts->at(i), etas->at(i), phis->at(i), masses->at(i));  // build the tau

      tau.pass_vloose_iso = pass_vloose_iso->at(i);
      tau.pass_loose_iso  = pass_loose_iso->at(i);
      tau.pass_medium_iso = pass_medium_iso->at(i);
      tau.pass_tight_iso  = pass_tight_iso->at(i);
      tau.pass_vtight_iso = pass_vtight_iso->at(i);

      taus.push_back(tau);
    }
  }

  // sort by pT
  std::sort(taus.begin(), taus.end(), [](Tau& p1, Tau& p2) -> bool { return p1.getPt() > p2.getPt(); });
  nGoodTau = taus.size();
}

#endif  // INTERFACE_TAU_FACTORY_H_
