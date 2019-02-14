// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_MUON_FACTORY_H_
#define INTERFACE_MUON_FACTORY_H_

#include <memory>
#include <vector>
#include "./muon.h"
#include "TTree.h"

typedef std::vector<Muon> VMuon;

class Muon_Factory {
 public:
  explicit Muon_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNMuon() { return nMu; }
  std::shared_ptr<VMuon> getMuons() { return std::make_shared<VMuon>(muons); }

 private:
  Int_t nMu;
  VMuon muons;
  std::vector<Float_t> *pts, *etas, *phis, *masses;
};

Muon_Factory::Muon_Factory(TTree *tree) {
  tree->SetBranchAddress("nTau", &nMu);
  tree->SetBranchAddress("muPt", &pts);
  tree->SetBranchAddress("muEta", &etas);
  tree->SetBranchAddress("muPhi", &phis);
  tree->SetBranchAddress("muMass", &masses);
}

void Muon_Factory::Run_Factory() {
  muons.clear();
  for (auto i = 0; i < nMu; i++) {
    auto muon = Muon(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    muons.push_back(muon);
  }
}

#endif  // INTERFACE_MUON_FACTORY_H_
