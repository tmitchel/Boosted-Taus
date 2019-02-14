// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_ELECTRON_FACTORY_H_
#define INTERFACE_ELECTRON_FACTORY_H_

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
  Int_t getNElectron() { return nEle; }
  std::shared_ptr<VElectron> getElectrons() { return std::make_shared<VElectron>(electrons); }

 private:
  Int_t nEle;
  VElectron electrons;
  std::vector<Float_t> *pts, *etas, *phis, *masses;
};

Electron_Factory::Electron_Factory(TTree *tree) {
  tree->SetBranchAddress("nTau", &nEle);
  tree->SetBranchAddress("elePt", &pts);
  tree->SetBranchAddress("eleEta", &etas);
  tree->SetBranchAddress("elePhi", &phis);
  tree->SetBranchAddress("eleMass", &masses);
}

void Electron_Factory::Run_Factory() {
  electrons.clear();
  for (auto i = 0; i < nEle; i++) {
    auto electron = Electron(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    electrons.push_back(electron);
  }
}

#endif  // INTERFACE_ELECTRON_FACTORY_H_
