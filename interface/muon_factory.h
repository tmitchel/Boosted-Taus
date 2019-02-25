// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_MUON_FACTORY_H_
#define INTERFACE_MUON_FACTORY_H_

#include <algorithm>
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
  Int_t getNTotalMuon() { return nMu; }
  Int_t getNGoodMuon() { return nGoodMu; }
  std::shared_ptr<VMuon> getMuons() { return std::make_shared<VMuon>(muons); }

 private:
  Int_t nMu, nGoodMu;
  VMuon muons;
  std::vector<Float_t> *pts, *etas, *phis, *energy;
};

Muon_Factory::Muon_Factory(TTree *tree) : pts(nullptr), etas(nullptr), phis(nullptr), energy(nullptr) {
  tree->SetBranchAddress("nTau", &nMu);
  tree->SetBranchAddress("muPt", &pts);
  tree->SetBranchAddress("muEta", &etas);
  tree->SetBranchAddress("muPhi", &phis);
  tree->SetBranchAddress("muEn", &energy);
}

void Muon_Factory::Run_Factory() {
  muons.clear();
  for (auto i = 0; i < nMu; i++) {
    auto muon = Muon(pts->at(i), etas->at(i), phis->at(i), energy->at(i));
    muons.push_back(muon);
  }

  // sort by pT
  std::sort(muons.begin(), muons.end(), [](Muon& p1, Muon& p2) -> bool { return p1.getPt() < p2.getPt(); });
  nGoodMu = muons.size();
}

#endif  // INTERFACE_MUON_FACTORY_H_
