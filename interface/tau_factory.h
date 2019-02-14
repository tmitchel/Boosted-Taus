// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_TAU_FACTORY_H_
#define INTERFACE_TAU_FACTORY_H_

#include <memory>
#include <vector>
#include "./tau.h"
#include "TTree.h"

typedef std::vector<Tau> VTau;

class Tau_Factory {
 public:
  explicit Tau_Factory(TTree*);
  void Run_Factory();

  // getters
  Int_t getNTau() { return nTau; }
  std::shared_ptr<VTau> getTaus() { return std::make_shared<VTau>(taus); }

 private:
  Int_t nTau;
  VTau taus;
  std::vector<Bool_t> *pass_vloose_iso, *pass_loose_iso, *pass_medium_iso,
      *pass_tight_iso, *pass_vtight_iso;  // all are MVArun2v2DBoldDMwLT
  std::vector<Float_t> *pts, *etas, *phis, *masses, *iso;
};

Tau_Factory::Tau_Factory(TTree* tree) {
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
    auto tau = Tau(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    tau.setLooseIso(pass_loose_iso->at(i));
    if (tau.getPt() > 20 && fabs(tau.getEta()) < 2.3 && tau.getLooseIso()) {
      taus.push_back(tau);
    }
  }
}

#endif  // INTERFACE_TAU_FACTORY_H_
