// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_JETS_FACTORY_H_
#define INTERFACE_JETS_FACTORY_H_

#include <memory>
#include <vector>
#include "./jets.h"
#include "TTree.h"

typedef std::vector<Jets> VJets;

class Jets_Factory {
 public:
  explicit Jets_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalJets() { return nJet; }
  Int_t getNGoodJets() { return nGoodJet; }
  std::shared_ptr<VJets> getJets() { return std::make_shared<VJets>(jets); }

 private:
  Int_t nJet, nGoodJet;
  VJets jets;
  std::vector<Float_t> *pts, *etas, *phis, *energy;
};

Jets_Factory::Jets_Factory(TTree *tree) : pts(nullptr), etas(nullptr), phis(nullptr), energy(nullptr) {
  tree->SetBranchAddress("nJet", &nJet);
  tree->SetBranchAddress("jetPt", &pts);
  tree->SetBranchAddress("jetEta", &etas);
  tree->SetBranchAddress("jetPhi", &phis);
  tree->SetBranchAddress("jetEn", &energy);
}

void Jets_Factory::Run_Factory() {
  jets.clear();
  for (auto i = 0; i < nJet; i++) {
    auto jet = Jets(pts->at(i), etas->at(i), phis->at(i), energy->at(i));
    jets.push_back(jet);
  }
  nGoodJet = jets.size();
}

#endif  // INTERFACE_JETS_FACTORY_H_
