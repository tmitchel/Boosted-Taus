// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_FACTORY_H_
#define INTERFACE_ELECTRON_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./electron.h"
#include "TTree.h"

typedef std::vector<Electron> VElectron;

class Electron_Factory {
 public:
  explicit Electron_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalElectron() { return nEle; }
  Int_t getNGoodElectron() { return nGoodEle; }
  std::shared_ptr<VElectron> getElectrons() { return std::make_shared<VElectron>(electrons); }

 private:
  Int_t nEle, nGoodEle;
  VElectron electrons;
  std::vector<Float_t> *pts, *etas, *phis, *energy;
};

Electron_Factory::Electron_Factory(TTree *tree) : pts(nullptr), etas(nullptr), phis(nullptr), energy(nullptr) {
  tree->SetBranchAddress("nTau", &nEle);
  tree->SetBranchAddress("elePt", &pts);
  tree->SetBranchAddress("eleEta", &etas);
  tree->SetBranchAddress("elePhi", &phis);
  tree->SetBranchAddress("eleEn", &energy);
}

void Electron_Factory::Run_Factory() {
  electrons.clear();
  for (auto i = 0; i < nEle; i++) {
    auto electron = Electron(pts->at(i), etas->at(i), phis->at(i), energy->at(i));
    electrons.push_back(electron);
  }

  // sort by pT
  std::sort(electrons.begin(), electrons.end(),
            [](TLorentzVector p1, TLorentzVector p2) -> bool { return p1.Pt() < p2.Pt(); });
  nGoodEle = electrons.size();
}

#endif  // INTERFACE_ELECTRON_FACTORY_H_
