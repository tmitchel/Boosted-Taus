// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_FACTORY_H_
#define INTERFACE_GEN_FACTORY_H_

#include <memory>
#include <vector>
#include "./gen.h"
#include "TTree.h"

typedef std::vector<Gen> VGen;

class Gen_Factory {
 public:
  explicit Gen_Factory(TTree*);
  void Run_Factory();

  // getters
  Int_t getNGen() { return nGen; }
  std::shared_ptr<VGen> getGens() { return std::make_shared<VGen>(gen_particles); }

 private:
  Int_t nGen;
  VGen gen_particles;
  std::vector<Int_t> *pids;
  std::vector<Float_t> *pts, *etas, *phis, *masses;
};

Gen_Factory::Gen_Factory(TTree* tree) {
  tree->SetBranchAddress("nMC", &nGen);
  tree->SetBranchAddress("mcPt", &pts);
  tree->SetBranchAddress("mcEta", &etas);
  tree->SetBranchAddress("mcPhi", &phis);
  tree->SetBranchAddress("mcMass", &masses);
  tree->SetBranchAddress("mcPID", &pids);
}

void Gen_Factory::Run_Factory() {
  gen_particles.clear();
  for (auto i = 0; i < nGen; i++) {
    auto gen = Gen(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    gen.pid = pids->at(i);
    gen_particles.push_back(gen);
  }
}

#endif  // INTERFACE_GEN_FACTORY_H_
