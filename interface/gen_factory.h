// Copyright [2019] Tyler Mitchell

#ifndef INTERFACE_GEN_FACTORY_H_
#define INTERFACE_GEN_FACTORY_H_

#include <algorithm>
#include <memory>
#include <vector>
#include "./gen.h"
#include "TTree.h"

typedef std::vector<Gen> VGen;

class Gen_Factory {
 public:
  explicit Gen_Factory(TTree *);
  void Run_Factory();

  // getters
  Int_t getNTotalGen() { return nGen; }
  Int_t getNGoodGen() { return nGoodGen; }
  Float_t getMET() { return genMET; }
  Float_t getMetPhi() { return genMETPhi; }
  TLorentzVector getMETP4() { return MET_p4; }
  std::shared_ptr<VGen> getGens() { return std::make_shared<VGen>(gen_particles); }
  std::shared_ptr<VGen> getGenJets() { return std::make_shared<VGen>(gen_jets); }

 private:
  Int_t nGen, nGoodGen;
  Float_t genMET, genMETPhi;
  VGen gen_particles, gen_jets;
  TLorentzVector MET_p4;
  std::vector<Int_t> *pids;
  std::vector<Float_t> *pts, *etas, *phis, *masses, *jetPt, *jetEta, *jetPhi, *jetEn;
};

Gen_Factory::Gen_Factory(TTree *tree)
    : pids(nullptr),
      pts(nullptr),
      etas(nullptr),
      phis(nullptr),
      masses(nullptr),
      jetPt(nullptr),
      jetEta(nullptr),
      jetPhi(nullptr),
      jetEn(nullptr) {
  tree->SetBranchAddress("nMC", &nGen);
  tree->SetBranchAddress("mcPt", &pts);
  tree->SetBranchAddress("mcEta", &etas);
  tree->SetBranchAddress("mcPhi", &phis);
  tree->SetBranchAddress("mcMass", &masses);
  tree->SetBranchAddress("mcPID", &pids);
  tree->SetBranchAddress("genMET", &genMET);
  tree->SetBranchAddress("genMETPhi", &genMETPhi);
  tree->SetBranchAddress("jetGenJetPt", &jetPt);
  tree->SetBranchAddress("jetGenJetEta", &jetEta);
  tree->SetBranchAddress("jetGenJetPhi", &jetPhi);
  tree->SetBranchAddress("jetGenJetEn", &jetEn);
}

void Gen_Factory::Run_Factory() {
  gen_particles.clear();
  gen_jets.clear();
  for (auto i = 0; i < nGen; i++) {
    auto gen = Gen(pts->at(i), etas->at(i), phis->at(i), masses->at(i));
    gen.pid = pids->at(i);
    gen_particles.push_back(gen);
  }

  // sort by pT
  std::sort(gen_particles.begin(), gen_particles.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() < p2.getPt(); });

  for (auto i = 0; i < jetPt->size(); i++) {
    gen_jets.push_back(Gen(jetPt->at(i), jetEta->at(i), jetPhi->at(i), jetEn->at(i), true));
  }

  // sort by pT
  std::sort(gen_jets.begin(), gen_jets.end(), [](Gen &p1, Gen &p2) -> bool { return p1.getPt() < p2.getPt(); });

  nGoodGen = gen_particles.size();
  MET_p4.SetPtEtaPhiM(genMET, 0, genMETPhi, 0);
}

#endif  // INTERFACE_GEN_FACTORY_H_
